#include <bits/stdc++.h>
using namespace std;
constexpr bool DEBUG = false;
constexpr bool SIM_MODE =
    false; // true にするとシミュレーションモード（初期入力以外の入力なし）
constexpr double TIME_LIMIT = 1.99;
// 連続同一手の打ち切り基準（実際の値は時間余裕係数をかけて使用）
// Optuna用: AHC_EARLY_EXIT_THRESHOLD_M2 / AHC_EARLY_EXIT_THRESHOLD_M8
// で上書き可能
constexpr int DEFAULT_EARLY_EXIT_THRESHOLD_M2 = 777;
constexpr int DEFAULT_EARLY_EXIT_THRESHOLD_M8 = 777;

constexpr int SHARED_PLAYOUT_PARAM_BLOCK_SIZE_TLAST = 1;
// MAX_PLAYOUT_DEPTH[U][M]: U×Mごとのプレイアウトの深さ制限
// U: index 0は未使用, 1〜5がU=1〜5に対応
// M: index 0,1は未使用, 2〜8がM=2〜8に対応
// 初期値はM別の旧値をコピー (optunaでU×M別にチューニング予定)
constexpr int DEFAULT_MAX_PLAYOUT_DEPTH[6][9] = {
    {0, 0, 7, 10, 11, 10, 12, 9, 10},   {0, 0, 16, 17, 17, 18, 17, 18, 12},
    {0, 0, 15, 12, 14, 15, 17, 15, 17}, {0, 0, 13, 14, 13, 15, 15, 16, 16},
    {0, 0, 16, 13, 19, 17, 12, 18, 20}, {0, 0, 13, 13, 15, 16, 16, 15, 16},
};
constexpr double PLAYOUT_TIME_RATIO = 1; // 最終ターン/最初ターンの時間比率
constexpr double PLAYOUT_TIME_EXP = 1;   // 補間の指数 (1.0=線形, >1で後半重視)

// === 自手評価パラメータ (U×Mごとのデフォルト値) ===
// U: index 0は未使用, 1〜5がU=1〜5に対応
// M: index 0,1は未使用, 2〜8がM=2〜8に対応
// 適用されないケースには1.0(乗算で無効)を設定

// --- 空きマス ---
// 空きマスの重み [U][M]
constexpr auto DEFAULT_SELF_EVAL_EMPTY = []() constexpr {
    array<array<double, 9>, 6> a{};
    for (int u = 0; u <= 5; u++)
        for (int m = 0; m <= 8; m++)
            a[u][m] = 1.3;
    a[1][2] = 0.118535223121764;
    a[1][3] = 0.44745515991867602;
    a[1][4] = 0.16489375157433001;
    a[1][5] = 1.1392165261763001;
    a[1][6] = 2.1693419109846199;
    a[1][7] = 1.37557250122516;
    a[1][8] = 1.16205496343096;
    a[2][2] = 0.038507346800044799;
    a[2][3] = 0.396676117987458;
    a[2][4] = 0.69352421895265903;
    a[2][5] = 1.73983629269603;
    a[2][6] = 1.57646485015289;
    a[2][7] = 1.5992147379570998;
    a[2][8] = 2.4156469903748334;
    a[3][2] = 0.35578409004732398;
    a[3][3] = 0.90208085726853304;
    a[3][4] = 1.0950740452811101;
    a[3][5] = 1.56662014836888;
    a[3][6] = 0.50469069747180195;
    a[3][7] = 0.98083568457679604;
    a[3][8] = 1.3292363077676908;
    a[4][2] = 0.14223432549488901;
    a[4][3] = 0.258317244929676;
    a[4][4] = 0.46919210327770799;
    a[4][5] = 0.60634256803377495;
    a[4][6] = 0.19787494931732;
    a[4][7] = 1.0454376834959165;
    a[4][8] = 2.5415386826171664;
    a[5][2] = 0.20676499822300101;
    a[5][3] = 0.63803484280563705;
    a[5][4] = 1.21936502617853;
    a[5][5] = 2.38344749367287;
    a[5][6] = 0.645233912623426;
    a[5][7] = 1.9482054670547995;
    a[5][8] = 0.31894222653080395;
    return a;
}();

// 自陣マスのレベル別ボーナス係数 [U][M][level]
constexpr auto DEFAULT_SELF_EVAL_OWN_LEVEL = []() constexpr {
    array<array<array<double, 6>, 9>, 6> a{};
    for (int u = 0; u <= 5; u++) {
        for (int m = 0; m <= 8; m++) {
            for (int lv = 0; lv <= 5; lv++)
                a[u][m][lv] = 1.0;
            if (u >= 1)
                a[u][m][u] = 0.3; // level == U
        }
    }
    a[1][2][1] = 0.54214500891738104;
    a[1][3][1] = 1.0602943747561;
    a[1][4][1] = 0.58839796743741002;
    a[1][5][1] = 2.46422805160099;
    a[1][6][1] = 0.28905037184393001;
    a[1][7][1] = 1.50435991838949;
    a[1][8][1] = 1.91494626468631;
    a[2][2][1] = 2.1284284730002301;
    a[2][2][2] = 0.83851613679009696;
    a[2][3][1] = 2.3151250142293001;
    a[2][3][2] = 1.77684782195576;
    a[2][4][1] = 2.3951286031892001;
    a[2][4][2] = 1.10026588223327;
    a[2][5][1] = 2.77344763119667;
    a[2][5][2] = 1.2896818346033001;
    a[2][6][1] = 2.3798166857532999;
    a[2][6][2] = 1.5488506024228601;
    a[2][7][1] = 1.8206948140674573;
    a[2][7][2] = 1.0865556482068315;
    a[2][8][1] = 1.358723797888002;
    a[2][8][2] = 1.0722203730968325;
    a[3][2][1] = 1.6282002358579;
    a[3][2][2] = 0.47785440353668901;
    a[3][2][3] = 0.053083727960141297;
    a[3][3][1] = 2.1100089066030301;
    a[3][3][2] = 0.82968996954132601;
    a[3][3][3] = 0.47876772112247301;
    a[3][4][1] = 1.4030764181403399;
    a[3][4][2] = 1.23504407672552;
    a[3][4][3] = 1.2156711690589901;
    a[3][5][1] = 1.5579569558898501;
    a[3][5][2] = 0.56661493494318205;
    a[3][5][3] = 0.38186189321797798;
    a[3][6][1] = 2.21864842048958;
    a[3][6][2] = 1.98122655273598;
    a[3][6][3] = 0.63243743047404499;
    a[3][7][1] = 1.3206969520103868;
    a[3][7][2] = 1.0168960083952918;
    a[3][7][3] = 0.85295096449504249;
    a[3][8][1] = 1.472581450330108;
    a[3][8][2] = 0.31188294935268379;
    a[3][8][3] = 0.067765021860923785;
    a[4][2][1] = 1.80110744822973;
    a[4][2][2] = 0.267468016116166;
    a[4][2][3] = 0.159479290840425;
    a[4][2][4] = 0.0508942862763999;
    a[4][3][1] = 2.2016777230526401;
    a[4][3][2] = 0.49653263204980302;
    a[4][3][3] = 0.39292282042721899;
    a[4][3][4] = 0.180014927881638;
    a[4][4][1] = 2.2068475037957;
    a[4][4][2] = 2.0277672891247098;
    a[4][4][3] = 1.1906349339686599;
    a[4][4][4] = 0.64290147909036599;
    a[4][5][1] = 1.4163199906814801;
    a[4][5][2] = 0.68607659693551803;
    a[4][5][3] = 0.63127290962256999;
    a[4][5][4] = 0.61272752641040895;
    a[4][6][1] = 2.01035689306666;
    a[4][6][2] = 1.39128641610145;
    a[4][6][3] = 1.2169271560391399;
    a[4][6][4] = 0.88965908413421302;
    a[4][7][1] = 2.8107928607802233;
    a[4][7][2] = 2.2975037980604376;
    a[4][7][3] = 1.2289734388443339;
    a[4][7][4] = 0.57502419929496129;
    a[4][8][1] = 1.8786114751659448;
    a[4][8][2] = 0.66143257012831913;
    a[4][8][3] = 0.26802210519996866;
    a[4][8][4] = 0.22907430734721518;
    a[5][2][1] = 1.2364892103564;
    a[5][2][2] = 0.53430087802972304;
    a[5][2][3] = 0.093295119148789601;
    a[5][2][4] = 0.017712919307002601;
    a[5][2][5] = 0.015472888905209601;
    a[5][3][1] = 1.3946341088049401;
    a[5][3][2] = 1.3204417124279599;
    a[5][3][3] = 0.52098650253559298;
    a[5][3][4] = 0.31637359394396902;
    a[5][3][5] = 0.15182259916246399;
    a[5][4][1] = 1.61496264222651;
    a[5][4][2] = 0.24623103221609199;
    a[5][4][3] = 0.16465520952705101;
    a[5][4][4] = 0.058001751163715899;
    a[5][4][5] = 0.043735205325648903;
    a[5][5][1] = 2.9931650149023099;
    a[5][5][2] = 2.79596956519755;
    a[5][5][3] = 2.7773826877214902;
    a[5][5][4] = 2.5897399948216502;
    a[5][5][5] = 2.09056483564523;
    a[5][6][1] = 1.18421155351663;
    a[5][6][2] = 1.1725493262366;
    a[5][6][3] = 1.16124363210821;
    a[5][6][4] = 1.0713952090569001;
    a[5][6][5] = 0.81235049679068405;
    a[5][7][1] = 2.5803031799156209;
    a[5][7][2] = 2.2221865957454083;
    a[5][7][3] = 1.3960955814225591;
    a[5][7][4] = 1.1589758885642287;
    a[5][7][5] = 0.87544794780018775;
    a[5][8][1] = 2.8540003481562817;
    a[5][8][2] = 1.834816144665899;
    a[5][8][3] = 1.1224484629020768;
    a[5][8][4] = 0.41819697064255035;
    a[5][8][5] = 0.23965934018753529;
    return a;
}();

// --- 敵陣マス ---
// トップ敵のレベル別ボーナス係数 [U][M][level]
// 旧: U=1→{0,2.5,0,0,0,0}, U!=1,M=2→M2_LEVEL_BONUS,
// U!=1,M!=2→TOP_ENEMY_LEVEL_BONUS
constexpr auto DEFAULT_SELF_EVAL_ENEMY_TOP_LEVEL = []() constexpr {
    constexpr array<double, 6> base = {1.0, 2.5, 2.0, 1.6, 1.4, 1.2};
    array<array<array<double, 6>, 9>, 6> a{};
    for (int u = 0; u <= 5; u++) {
        for (int m = 0; m <= 8; m++) {
            for (int lv = 0; lv <= 5; lv++)
                a[u][m][lv] = 1.0;
            for (int lv = 1; lv <= u; lv++)
                a[u][m][lv] = base[lv];
        }
    }
    a[1][2][1] = 1.2693748915146901;
    a[1][3][1] = 2.11263713217329;
    a[1][4][1] = 1.92768002245625;
    a[1][5][1] = 2.9391589670009499;
    a[1][6][1] = 2.61333989291029;
    a[1][7][1] = 1.58654412762881;
    a[1][8][1] = 2.18998996750928;
    a[2][2][1] = 2.6761333743217999;
    a[2][2][2] = 2.2225835508399499;
    a[2][3][1] = 2.2174213787128698;
    a[2][3][2] = 2.06880993500527;
    a[2][4][1] = 2.4120262704396702;
    a[2][4][2] = 2.25992063900962;
    a[2][5][1] = 2.90986467506018;
    a[2][5][2] = 1.7803055296752599;
    a[2][6][1] = 2.6802435380988801;
    a[2][6][2] = 2.35673985795265;
    a[2][7][1] = 2.4303895404057969;
    a[2][7][2] = 2.0821673864458692;
    a[2][8][1] = 2.0361373715653346;
    a[2][8][2] = 1.5006722886220427;
    a[3][2][1] = 2.4370103522389899;
    a[3][2][2] = 1.6731688734460901;
    a[3][2][3] = 1.3137738550984099;
    a[3][3][1] = 1.99184778514751;
    a[3][3][2] = 1.53832988650518;
    a[3][3][3] = 1.4623546871455999;
    a[3][4][1] = 2.66997678177189;
    a[3][4][2] = 2.5885723884425902;
    a[3][4][3] = 1.67042051815424;
    a[3][5][1] = 2.9686763052473299;
    a[3][5][2] = 2.2239785266690699;
    a[3][5][3] = 1.92592615281922;
    a[3][6][1] = 2.09038796378923;
    a[3][6][2] = 1.2032833889196599;
    a[3][6][3] = 1.1936535498182399;
    a[3][7][1] = 2.8851031563129541;
    a[3][7][2] = 2.6773503297831187;
    a[3][7][3] = 2.5462914460396364;
    a[3][8][1] = 1.0837156558414904;
    a[3][8][2] = 0.79603662315825119;
    a[3][8][3] = 0.52230988823689861;
    a[4][2][1] = 1.5295631287646601;
    a[4][2][2] = 1.2822571653533901;
    a[4][2][3] = 0.77177968115698503;
    a[4][2][4] = 0.77103261375708898;
    a[4][3][1] = 2.7545977460691602;
    a[4][3][2] = 1.6335042732751499;
    a[4][3][3] = 1.4775303996243301;
    a[4][3][4] = 0.45488764144424598;
    a[4][4][1] = 2.7765946259116099;
    a[4][4][2] = 2.2364620727019102;
    a[4][4][3] = 2.11552940294836;
    a[4][4][4] = 2.0809225039113999;
    a[4][5][1] = 2.6986566456558698;
    a[4][5][2] = 1.8614797977665301;
    a[4][5][3] = 1.10340336375833;
    a[4][5][4] = 1.0730897887525599;
    a[4][6][1] = 2.4020394593875598;
    a[4][6][2] = 2.2626532099298702;
    a[4][6][3] = 2.1967599563414;
    a[4][6][4] = 2.00041734117353;
    a[4][7][1] = 2.3565943830265539;
    a[4][7][2] = 2.1635330425665176;
    a[4][7][3] = 1.717487241666815;
    a[4][7][4] = 1.4914537241433872;
    a[4][8][1] = 2.4561180108053606;
    a[4][8][2] = 2.2286585193726105;
    a[4][8][3] = 1.4695388538876404;
    a[4][8][4] = 1.3942697213003983;
    a[5][2][1] = 1.6501838457600699;
    a[5][2][2] = 1.08081157190802;
    a[5][2][3] = 0.83769738581337405;
    a[5][2][4] = 0.79932488518780498;
    a[5][2][5] = 0.60165752836511699;
    a[5][3][1] = 2.2786188821438;
    a[5][3][2] = 1.9580568776753999;
    a[5][3][3] = 1.7383695555557599;
    a[5][3][4] = 1.55457344970789;
    a[5][3][5] = 1.34019500234613;
    a[5][4][1] = 2.1061869707106098;
    a[5][4][2] = 1.9132041070047801;
    a[5][4][3] = 1.8856032434882699;
    a[5][4][4] = 1.6318720326790399;
    a[5][4][5] = 1.5704058448176701;
    a[5][5][1] = 2.47944185748426;
    a[5][5][2] = 2.3833889037985601;
    a[5][5][3] = 2.2928350859360598;
    a[5][5][4] = 1.8694138949567001;
    a[5][5][5] = 0.53310708536205698;
    a[5][6][1] = 2.7820739049293199;
    a[5][6][2] = 2.7679988305553702;
    a[5][6][3] = 1.6719084293468101;
    a[5][6][4] = 1.45171789926073;
    a[5][6][5] = 1.2565908693269101;
    a[5][7][1] = 2.5049520118722763;
    a[5][7][2] = 2.2335046586076852;
    a[5][7][3] = 1.6623041197076249;
    a[5][7][4] = 1.397422580644363;
    a[5][7][5] = 0.97265312744828325;
    a[5][8][1] = 1.4139765335583803;
    a[5][8][2] = 1.132296708552025;
    a[5][8][3] = 0.91754194546139911;
    a[5][8][4] = 0.3710619715590695;
    a[5][8][5] = 0.19464957449424966;
    return a;
}();
// 非トップ敵のレベル別ボーナス係数 [U][M][level]
constexpr auto DEFAULT_SELF_EVAL_ENEMY_NOT_TOP_LEVEL = []() constexpr {
    constexpr array<double, 6> base = {1.0, 1.2, 0.6, 0.4, 0.3, 0.2};
    array<array<array<double, 6>, 9>, 6> a{};
    for (int u = 0; u <= 5; u++) {
        for (int m = 0; m <= 8; m++) {
            for (int lv = 0; lv <= 5; lv++)
                a[u][m][lv] = 1.0;
            for (int lv = 1; lv <= u; lv++)
                a[u][m][lv] = base[lv];
        }
    }
    a[1][3][1] = 0.770448373379234;
    a[1][4][1] = 1.2849238374702601;
    a[1][5][1] = 2.6642015026650001;
    a[1][6][1] = 2.5486485632999401;
    a[1][7][1] = 1.24204770140055;
    a[1][8][1] = 0.0607796384880843;
    a[2][3][1] = 2.1747044803529199;
    a[2][3][2] = 1.30703933861763;
    a[2][4][1] = 1.9568508096886399;
    a[2][4][2] = 1.25292806973537;
    a[2][5][1] = 0.711796298426288;
    a[2][5][2] = 0.12548755381016699;
    a[2][6][1] = 1.50135385897826;
    a[2][6][2] = 1.2431351422880099;
    a[2][7][1] = 1.0919512676797494;
    a[2][7][2] = 0.16211833121051972;
    a[2][8][1] = 0.89121818160838062;
    a[2][8][2] = 0.57635511469253276;
    a[3][3][1] = 1.5992219608559901;
    a[3][3][2] = 0.31692022409974202;
    a[3][3][3] = 0.12871277230315301;
    a[3][4][1] = 0.99484880981986701;
    a[3][4][2] = 0.25966892088763099;
    a[3][4][3] = 0.054213315004770703;
    a[3][5][1] = 1.12670375839763;
    a[3][5][2] = 0.45607354686229901;
    a[3][5][3] = 0.116164989970985;
    a[3][6][1] = 1.5406265803576999;
    a[3][6][2] = 0.835104649347155;
    a[3][6][3] = 0.30522240400793399;
    a[3][7][1] = 1.3471653046171732;
    a[3][7][2] = 1.1728657938812788;
    a[3][7][3] = 0.48131839760956679;
    a[3][8][1] = 0.79380650205619174;
    a[3][8][2] = 0.27627842017308274;
    a[3][8][3] = 0.15767192228781293;
    a[4][3][1] = 0.107368979197019;
    a[4][3][2] = 0.041673422349549301;
    a[4][3][3] = 0.024403664606807499;
    a[4][3][4] = 0.017176045469447;
    a[4][4][1] = 1.7373924326052601;
    a[4][4][2] = 0.96731868554782696;
    a[4][4][3] = 0.41051384954125503;
    a[4][4][4] = 0.242543941300258;
    a[4][5][1] = 0.49088577595410299;
    a[4][5][2] = 0.47976529553667902;
    a[4][5][3] = 0.040833002379833901;
    a[4][5][4] = 0.0060987040708566798;
    a[4][6][1] = 2.0754203756224001;
    a[4][6][2] = 1.83274628390804;
    a[4][6][3] = 0.86150947519620003;
    a[4][6][4] = 0.79283116682084098;
    a[4][7][1] = 2.3182419540314485;
    a[4][7][2] = 0.45794516854446898;
    a[4][7][3] = 0.224336720213867;
    a[4][7][4] = 0.2047355914276707;
    a[4][8][1] = 0.023519017052162777;
    a[4][8][2] = 0.016975158848208223;
    a[4][8][3] = 0.013119718831213567;
    a[4][8][4] = 0.012304808732816135;
    a[5][3][1] = 0.67527120257055995;
    a[5][3][2] = 0.22284879290045001;
    a[5][3][3] = 0.108419588050328;
    a[5][3][4] = 0.0438088165827408;
    a[5][3][5] = 0.026116219381860602;
    a[5][4][1] = 1.3223324921903199;
    a[5][4][2] = 1.11939499312304;
    a[5][4][3] = 0.32099983827044198;
    a[5][4][4] = 0.095978126859693702;
    a[5][4][5] = 0.057637800654275602;
    a[5][5][1] = 0.494735897072902;
    a[5][5][2] = 0.0086276709121740607;
    a[5][5][3] = 0.0052601933154516403;
    a[5][5][4] = 0.00147330715021665;
    a[5][5][5] = 0.00126250250637604;
    a[5][6][1] = 0.48537616552568402;
    a[5][6][2] = 0.39366542518404002;
    a[5][6][3] = 0.31538253012857798;
    a[5][6][4] = 0.30293587800726701;
    a[5][6][5] = 0.299866736491542;
    a[5][7][1] = 1.3519059694096571;
    a[5][7][2] = 0.21974519991524655;
    a[5][7][3] = 0.13722616803570697;
    a[5][7][4] = 0.071609220880690438;
    a[5][7][5] = 0.034746152492605993;
    a[5][8][1] = 0.24770772977360822;
    a[5][8][2] = 0.10200551837118077;
    a[5][8][3] = 0.067564928621932599;
    a[5][8][4] = 0.036690092076852365;
    a[5][8][5] = 0.03060505724590145;
    return a;
}();
// --- greedy乱数・強化制限 ---
// 最初のXターンは自陣強化しない [U][M]
constexpr int DEFAULT_NO_REINFORCE_TURNS[6][9] = {
    {5, 5, 5, 5, 5, 5, 5, 5, 5},       {5, 5, 8, 12, 25, 25, 27, 25, 35},
    {5, 5, 4, 17, 11, 18, 12, 10, 12}, {5, 5, 4, 9, 15, 14, 15, 13, 10},
    {5, 5, 7, 15, 11, 5, 13, 10, 6},   {5, 5, 4, 10, 8, 4, 11, 8, 14},
};
// greedy乱数の下限 [U][M] (上限は1.0に正規化済み)
constexpr auto DEFAULT_SELF_EVAL_NOISE_LO = []() constexpr {
    array<array<double, 9>, 6> a{};
    for (int u = 0; u <= 5; u++)
        for (int m = 0; m <= 8; m++)
            a[u][m] = 0.3;
    a[1][2] = 0.287947366208328;
    a[1][3] = 0.207523395622808;
    a[1][4] = 0.160729542319931;
    a[1][5] = 0.205584841586332;
    a[1][6] = 0.12198378412991399;
    a[1][7] = 0.150039676394175;
    a[1][8] = 0.205738407425332;
    a[2][2] = 0.29318189558520102;
    a[2][3] = 0.042741696135959399;
    a[2][4] = 0.093335054449672702;
    a[2][5] = 0.050501420571493399;
    a[2][6] = 0.115954864933389;
    a[2][7] = 0.10636233344531065;
    a[2][8] = 0.71931086469928973;
    a[3][2] = 0.61058628315363195;
    a[3][3] = 0.17495955982422901;
    a[3][4] = 0.030809470249852498;
    a[3][5] = 0.40549900722441801;
    a[3][6] = 0.0023371980862823901;
    a[3][7] = 0.34965784977375397;
    a[3][8] = 0.08260254520737903;
    a[4][2] = 0.24785339690062999;
    a[4][3] = 0.051272444524312698;
    a[4][4] = 0.27335719105744799;
    a[4][5] = 0.58381325801633799;
    a[4][6] = 0.21796735182327101;
    a[4][7] = 0.093397903253708475;
    a[4][8] = 0.16684735217837079;
    a[5][2] = 0.33877255284689101;
    a[5][3] = 0.46950801256843699;
    a[5][4] = 0.32934905886244498;
    a[5][5] = 0.25515250036591902;
    a[5][6] = 0.13973618030420501;
    a[5][7] = 0.17372519746467174;
    a[5][8] = 0.0602318929860826;
    return a;
}();
constexpr auto DEFAULT_UCB_EXPLORATION_C = []() constexpr {
    array<array<double, 9>, 6> a{};
    for (int u = 0; u <= 5; u++)
        for (int m = 0; m <= 8; m++)
            a[u][m] = 0.7;
    return a;
}();

// === 環境変数によるパラメータ上書き ===
// true にすると環境変数からの読み込みを有効化
// optuna実行時: true, 通常提出時: false
constexpr bool USE_ENV_OVERRIDE = false;

// 実行時に使用するパラメータ (init_params で設定)
static int CURRENT_MAX_PLAYOUT_DEPTH;
static int CURRENT_EARLY_EXIT_THRESHOLD_M2;
static int CURRENT_EARLY_EXIT_THRESHOLD_M8;
// 空きマス
static double CURRENT_SELF_EVAL_EMPTY;
static double CURRENT_SELF_EVAL_OWN_LEVEL[6];
// 敵陣マス
static double CURRENT_SELF_EVAL_ENEMY_TOP_LEVEL[6];
static double CURRENT_SELF_EVAL_ENEMY_NOT_TOP_LEVEL[6];
// greedy乱数・強化制限
static double CURRENT_SELF_EVAL_NOISE_LO;
static int CURRENT_NO_REINFORCE_TURNS;
// UCB探索定数
static double CURRENT_UCB_EXPLORATION_C;

// 環境変数から double を読む。見つからなければ default_val を返す
static double read_env_double(const char *name, double default_val) {
    const char *val = getenv(name);
    if (val == nullptr)
        return default_val;
    return atof(val);
}

// 環境変数から int を読む。見つからなければ default_val を返す
static int read_env_int(const char *name, int default_val) {
    const char *val = getenv(name);
    if (val == nullptr)
        return default_val;
    return atoi(val);
}

static int clamp_early_exit_threshold(int v) { return max(100, min(1500, v)); }

// U×Mに応じたパラメータを初期化 (input.read() の後に呼ぶ)
static void init_params(int M, int U) {
    CURRENT_MAX_PLAYOUT_DEPTH = DEFAULT_MAX_PLAYOUT_DEPTH[U][M];
    CURRENT_EARLY_EXIT_THRESHOLD_M2 = DEFAULT_EARLY_EXIT_THRESHOLD_M2;
    CURRENT_EARLY_EXIT_THRESHOLD_M8 = DEFAULT_EARLY_EXIT_THRESHOLD_M8;
    CURRENT_SELF_EVAL_EMPTY = DEFAULT_SELF_EVAL_EMPTY[U][M];
    memcpy(CURRENT_SELF_EVAL_OWN_LEVEL,
           DEFAULT_SELF_EVAL_OWN_LEVEL[U][M].data(),
           sizeof(CURRENT_SELF_EVAL_OWN_LEVEL));
    memcpy(CURRENT_SELF_EVAL_ENEMY_TOP_LEVEL,
           DEFAULT_SELF_EVAL_ENEMY_TOP_LEVEL[U][M].data(),
           sizeof(CURRENT_SELF_EVAL_ENEMY_TOP_LEVEL));
    memcpy(CURRENT_SELF_EVAL_ENEMY_NOT_TOP_LEVEL,
           DEFAULT_SELF_EVAL_ENEMY_NOT_TOP_LEVEL[U][M].data(),
           sizeof(CURRENT_SELF_EVAL_ENEMY_NOT_TOP_LEVEL));
    CURRENT_SELF_EVAL_NOISE_LO = DEFAULT_SELF_EVAL_NOISE_LO[U][M];
    CURRENT_NO_REINFORCE_TURNS = DEFAULT_NO_REINFORCE_TURNS[U][M];
    CURRENT_UCB_EXPLORATION_C = DEFAULT_UCB_EXPLORATION_C[U][M];
    // 早期終了閾値は USE_ENV_OVERRIDE と無関係に環境変数から受け取る
    CURRENT_EARLY_EXIT_THRESHOLD_M2 = clamp_early_exit_threshold(read_env_int(
        "AHC_EARLY_EXIT_THRESHOLD_M2", CURRENT_EARLY_EXIT_THRESHOLD_M2));
    CURRENT_EARLY_EXIT_THRESHOLD_M8 = clamp_early_exit_threshold(read_env_int(
        "AHC_EARLY_EXIT_THRESHOLD_M8", CURRENT_EARLY_EXIT_THRESHOLD_M8));
    if constexpr (USE_ENV_OVERRIDE) {
        CURRENT_MAX_PLAYOUT_DEPTH =
            read_env_int("AHC_MAX_PLAYOUT_DEPTH", CURRENT_MAX_PLAYOUT_DEPTH);
        CURRENT_UCB_EXPLORATION_C =
            read_env_double("AHC_UCB_EXPLORATION_C", CURRENT_UCB_EXPLORATION_C);
        CURRENT_SELF_EVAL_EMPTY =
            read_env_double("AHC_SELF_EVAL_EMPTY", CURRENT_SELF_EVAL_EMPTY);
        for (int i = 1; i <= 5; i++) {
            char buf[64];
            snprintf(buf, sizeof(buf), "AHC_SELF_EVAL_OWN_LEVEL_%d", i);
            CURRENT_SELF_EVAL_OWN_LEVEL[i] =
                read_env_double(buf, CURRENT_SELF_EVAL_OWN_LEVEL[i]);
            snprintf(buf, sizeof(buf), "AHC_SELF_EVAL_ENEMY_TOP_LEVEL_%d", i);
            CURRENT_SELF_EVAL_ENEMY_TOP_LEVEL[i] =
                read_env_double(buf, CURRENT_SELF_EVAL_ENEMY_TOP_LEVEL[i]);
            snprintf(buf, sizeof(buf), "AHC_SELF_EVAL_ENEMY_NOT_TOP_LEVEL_%d",
                     i);
            CURRENT_SELF_EVAL_ENEMY_NOT_TOP_LEVEL[i] =
                read_env_double(buf, CURRENT_SELF_EVAL_ENEMY_NOT_TOP_LEVEL[i]);
        }
        CURRENT_SELF_EVAL_NOISE_LO = read_env_double(
            "AHC_SELF_EVAL_NOISE_LO", CURRENT_SELF_EVAL_NOISE_LO);
        CURRENT_NO_REINFORCE_TURNS =
            read_env_int("AHC_NO_REINFORCE_TURNS", CURRENT_NO_REINFORCE_TURNS);
    }
}

// Macros
#define el '\n'
#define all(v) (v).begin(), (v).end()
using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;
using u128 = __uint128_t;
#define rep(i, n) for (i64 i = 0; i < (i64)(n); i++)
const array<int, 8> DX = {0, 0, -1, 1, -1, -1, 1, 1};
const array<int, 8> DY = {-1, 1, 0, 0, -1, 1, -1, 1};

struct LocalSplit3x3 {
    static constexpr array<u8, 8> ADJ = {
        (u8)((1u << 1) | (1u << 3)), // 0: (-1,-1)
        (u8)((1u << 0) | (1u << 2)), // 1: (-1,0)
        (u8)((1u << 1) | (1u << 4)), // 2: (-1,1)
        (u8)((1u << 0) | (1u << 5)), // 3: (0,-1)
        (u8)((1u << 2) | (1u << 7)), // 4: (0,1)
        (u8)((1u << 3) | (1u << 6)), // 5: (1,-1)
        (u8)((1u << 5) | (1u << 7)), // 6: (1,0)
        (u8)((1u << 4) | (1u << 6)), // 7: (1,1)
    };
    static constexpr u8 SIDE_MASK =
        (u8)((1u << 1) | (1u << 3) | (1u << 4) | (1u << 6));

    static constexpr array<u8, 256> SPLIT_LUT = []() constexpr {
        array<u8, 256> lut{};
        for (int m = 0; m < 256; m++) {
            u8 mask = (u8)m;
            if (std::popcount((unsigned)(mask & SIDE_MASK)) <= 1) {
                lut[m] = 0;
                continue;
            }
            int start = std::countr_zero((unsigned)mask);
            u8 visited = (u8)(1u << start);
            u8 q = visited;
            while (q) {
                int v = std::countr_zero((unsigned)q);
                q &= (u8)(q - 1);
                u8 nbs = (u8)(ADJ[v] & mask & (u8)~visited);
                visited |= nbs;
                q |= nbs;
            }
            lut[m] = (visited != mask);
        }
        return lut;
    }();
};

// Random
struct RNG {
    uint64_t s[2];
    RNG(u64 seed) { reset(seed); }
    RNG() { reset(time(0)); }
    using result_type = u32;
    static constexpr u32 min() { return numeric_limits<u32>::min(); }
    static constexpr u32 max() { return numeric_limits<u32>::max(); }
    u32 operator()() { return randomInt32(); }
    static __attribute__((always_inline)) inline uint64_t rotl(const uint64_t x,
                                                               int k) {
        return (x << k) | (x >> (64 - k));
    }
    inline void reset(u64 seed) {
        struct splitmix64_state {
            u64 s;
            u64 splitmix64() {
                u64 result = (s += 0x9E3779B97f4A7C15);
                result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
                result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
                return result ^ (result >> 31);
            }
        };
        splitmix64_state sm{seed};
        s[0] = sm.splitmix64();
        s[1] = sm.splitmix64();
    }
    uint64_t next() {
        const uint64_t s0 = s[0];
        uint64_t s1 = s[1];
        const uint64_t result = rotl(s0 * 5, 7) * 9;
        s1 ^= s0;
        s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
        s[1] = rotl(s1, 37);                   // c
        return result;
    }
    inline u32 randomInt32() { return next(); }
    inline u64 randomInt64() { return next(); }
    inline u32 random32(u32 r) { return (((u64)randomInt32()) * r) >> 32; }
    inline u64 random64(u64 r) { return randomInt64() % r; }
    inline u32 randomRange32(u32 l, u32 r) { return l + random32(r - l + 1); }
    inline u64 randomRange64(u64 l, u64 r) { return l + random64(r - l + 1); }
    inline double randomDouble() {
        return (double)randomInt32() / 4294967296.0;
    }
    inline double randomDoubleOpen01() {
        return (randomInt32() + 1.0) / 4294967297.0;
    }
    inline float randomFloat() { return (float)randomInt32() / 4294967296.0; }
    inline double randomRangeDouble(double l, double r) {
        return l + randomDouble() * (r - l);
    }
    inline double randomGaussian(double mean = 0.0, double stddev = 1.0) {
        double u1 = randomDoubleOpen01();
        double u2 = randomDouble();
        double z = sqrt(-2.0 * log(u1)) * cos(2.0 * 3.141592653589793238 * u2);
        return mean + stddev * z;
    }
    template <class T> void shuffle(vector<T> &v) {
        i32 sz = v.size();
        for (i32 i = sz; i > 1; i--) {
            i32 p = random32(i);
            swap(v[i - 1], v[p]);
        }
    }
    template <class T> void shuffle(T *fr, T *to) {
        i32 sz = distance(fr, to);
        for (int i = sz; i > 1; i--) {
            int p = random32(i);
            swap(fr[i - 1], fr[p]);
        }
    }
    template <class T> inline int sample_index(vector<T> const &v) {
        return random32(v.size());
    }
    template <class T> inline T sample(vector<T> const &v) {
        return v[sample_index(v)];
    }
} rng(time(0) + 'R' + 'e' + 'i' + 'k' + 'a' + 5 / 17);
// Timer
struct Timer {
    chrono::steady_clock::time_point t_begin;
    Timer() { t_begin = chrono::steady_clock::now(); }
    void reset() { t_begin = chrono::steady_clock::now(); }
    float elapsed() const {
        return chrono::duration<float>(chrono::steady_clock::now() - t_begin)
            .count();
    }
} timer;
// Debug
static inline void debug() { cerr << "\n"; }
template <class T, class... V> void debug(T const &t, V const &...v) {
    if constexpr (DEBUG) {
        cerr << t;
        if (sizeof...(v)) {
            cerr << ", ";
        }
        debug(v...);
    }
}
// u128 helpers
static inline int popcount128(u128 x) {
    return __builtin_popcountll((u64)x) + __builtin_popcountll((u64)(x >> 64));
}
static inline int ctz128(u128 x) {
    u64 lo = (u64)x;
    return lo ? __builtin_ctzll(lo) : 64 + __builtin_ctzll((u64)(x >> 64));
}

struct Init {
    Init() {
        ios::sync_with_stdio(0);
        cin.tie(0);
    }
} init;

// ==========================================
// Constants
// ==========================================
constexpr int MAXN = 10;
constexpr int MAXM = 8;
constexpr int MAXCELLS = MAXN * MAXN;
constexpr auto NO_ENEMY_PROB_INIT = []() constexpr {
    array<float, MAXCELLS> a{};
    for (int i = 0; i < MAXCELLS; i++)
        a[i] = 1.0f;
    return a;
}();

static i8 GLOBAL_LOCAL8_NEIGHBOR_ID[MAXN + 1][MAXCELLS][8];
static bool GLOBAL_LOCAL8_NEIGHBOR_READY[MAXN + 1];

static void init_global_local8_neighbor_id(int N) {
    if (GLOBAL_LOCAL8_NEIGHBOR_READY[N])
        return;
    static constexpr int dx8[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    static constexpr int dy8[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            int id = x * N + y;
            for (int k = 0; k < 8; k++) {
                int nx = x + dx8[k], ny = y + dy8[k];
                GLOBAL_LOCAL8_NEIGHBOR_ID[N][id][k] =
                    ((unsigned)nx < (unsigned)N && (unsigned)ny < (unsigned)N)
                        ? (i8)(nx * N + ny)
                        : (i8)-1;
            }
        }
    }
    GLOBAL_LOCAL8_NEIGHBOR_READY[N] = true;
}

// 粒子フィルター定数
constexpr int N_PARTICLES = 500;
constexpr double LW_A = 0.99;               // Liu-West 収縮パラメータ
double LW_H = sqrt(1 - LW_A * LW_A);        // sqrt(1 - LW_A^2)
constexpr double ESS_THRESHOLD_RATIO = 0.5; // ESS < N*ratio でリサンプル

// eps離散グリッド (現行方式を維持)
constexpr int N_EPS_F = 17;
constexpr int N_EPS_SAMPLES = 100; // eps更新時のモンテカルロサンプル数
static double EPS_DBL_F[N_EPS_F];

static void init_eps_table() {
    for (int i = 0; i < N_EPS_F; i++) {
        EPS_DBL_F[i] = 0.1 + i * 0.025;
    }
}

// ==========================================
// 候補手リスト
// ==========================================
struct CandList {
    i8 ids[MAXCELLS];
    int n = 0;
    void clear() { n = 0; }
    void push_id(int id) { ids[n++] = id; }
    int random_pick_id() const { return ids[rng.random32(n)]; }
};

// ==========================================
// Input
// ==========================================
struct Input {
    int N, M, T, U;
    int V[MAXCELLS];
    i8 start_pos[MAXM];

    void read() {
        cin >> N >> M >> T >> U;
        rep(i, N) rep(j, N) cin >> V[i * N + j];
        rep(p, M) {
            int x, y;
            cin >> x >> y;
            start_pos[p] = x * N + y;
        }
    }
} input;

// ==========================================
// GameState
// ==========================================
struct GameState {
    // === 動的状態 (copy_dynamic_from で一括コピーされる連続領域) ===
    i8 owner[MAXCELLS];
    i8 level[MAXCELLS];
    i8 pos[MAXM];
    i32 score[MAXM];
    u128 owner_mask[MAXM]; // owner_mask[p] = プレイヤーpの所有マスク,
                           // owner_mask[MAXM]は未使用(sentinel)
    // === キャッシュ (copy_dynamic_from でコピーされる動的状態) ===
    u128 reachable_mask[MAXM]; // reachable_mask[p] =
                               // pos[p]からBFSした到達可能領土マスク
    u128 cand_base[MAXM];      // cand_base[p] = reachable_mask[p] |
                               // expand(reachable_mask[p])
    u128 players_mask;         // 全プレイヤー駒位置のビット和
    i8 top_enemy;              // 敵プレイヤーのうち、スコアが最大のプレイヤー
                               // (自分を除く)
    // === 静的トポロジー (init時に1度だけ設定、以降変化しない) ===
    i8 cell_x[MAXCELLS], cell_y[MAXCELLS];
    i8 adj_deg[MAXCELLS];
    i8 adj[MAXCELLS][4];
    u64 self_adj_mask_lo[MAXCELLS];
    u64 self_adj_mask_hi[MAXCELLS];
    u128 board_mask;                 // 有効なマス全体のマスク (100ビット)
    u128 col0_mask;                  // 列0のマスク (左シフト回り込み防止)
    u128 col_last_mask;              // 列N-1のマスク (右シフト回り込み防止)
    u128 self_adj_mask128[MAXCELLS]; // セルiと隣接セルのマスク(u128版)
    int N, M, U, C;
    // プレイヤー0の候補セルのうち、敵が誰も来ない確率
    // run_playoutのsループ先頭で fill で初期化されるため copy_dynamic_from
    // の対象外
    float no_enemy_prob[MAXCELLS];

    __attribute__((always_inline)) inline int to_id(int x, int y) const {
        return x * N + y;
    }
    __attribute__((always_inline)) inline int id_x(int id) const {
        return cell_x[id];
    }
    __attribute__((always_inline)) inline int id_y(int id) const {
        return cell_y[id];
    }

    __attribute__((always_inline)) inline u128 expand(u128 mask) const {
        return ((mask >> N) | (mask << N) | ((mask >> 1) & ~col_last_mask) |
                ((mask << 1) & ~col0_mask)) &
               board_mask;
    }

    // reachable_mask[p] と cand_base[p] を再計算
    // (owner_mask[p] が変わった直後に呼ぶ)
    __attribute__((always_inline)) inline void rebuild_reachable_mask(int p) {
        u128 owned = owner_mask[p];
        u128 reachable = (u128)1 << pos[p];
        u128 prev = 0;
        while (reachable != prev) {
            prev = reachable;
            reachable |= expand(reachable) & owned;
        }
        reachable_mask[p] = reachable;
        cand_base[p] = reachable | expand(reachable); // expand は1回だけ
    }

    // キャッシュ済み cand_base と players_mask から候補手マスクを O(1)
    // で返す
    __attribute__((always_inline)) inline u128 get_cand_mask(int player) const {
        // cand_base[player] = reachable | expand(reachable)
        // (expand計算済み) players_mask
        // から自分のビット(pos[player])を除いた集合を除外する players_mask
        // ^ (1<<pos[player]): 自分のビットをOFF,
        // 他プレイヤービットはそのまま
        u128 cands = cand_base[player];
        cands &= ~(players_mask ^ ((u128)1 << pos[player]));
        return cands & board_mask;
    }

    void get_candidates_from_mask(u128 mask, CandList &cands) const {
        cands.clear();
        while (mask) {
            int b = ctz128(mask);
            cands.push_id(b);
            mask &= mask - 1; // u128でもOK
        }
    }

    void init() {
        N = input.N;
        M = input.M;
        U = input.U;
        C = N * N;
        init_global_local8_neighbor_id(N);
        memset(owner, -1, sizeof(owner));
        memset(level, 0, sizeof(level));
        memset(score, 0, sizeof(score));

        for (int x = 0; x < N; x++) {
            for (int y = 0; y < N; y++) {
                int id = to_id(x, y);
                cell_x[id] = x;
                cell_y[id] = y;
                int deg = 0;
                for (int d = 0; d < 4; d++) {
                    int nx = x + DX[d], ny = y + DY[d];
                    if ((unsigned)nx < (unsigned)N &&
                        (unsigned)ny < (unsigned)N)
                        adj[id][deg++] = to_id(nx, ny);
                }
                adj_deg[id] = deg;
                u64 lo = 0, hi = 0;
                if (id < 64)
                    lo |= (1ULL << id);
                else
                    hi |= (1ULL << (id - 64));
                for (int k = 0; k < deg; k++) {
                    int nv = adj[id][k];
                    if (nv < 64)
                        lo |= (1ULL << nv);
                    else
                        hi |= (1ULL << (nv - 64));
                }
                self_adj_mask_lo[id] = lo;
                self_adj_mask_hi[id] = hi;
            }
        }

        // board_mask, col0_mask, col_last_mask を設定
        board_mask = ((u128)1 << C) - 1;
        col0_mask = 0;
        col_last_mask = 0;
        for (int r = 0; r < N; r++) {
            col0_mask |= (u128)1 << (r * N);
            col_last_mask |= (u128)1 << (r * N + N - 1);
        }
        // self_adj_mask128 を設定
        for (int id = 0; id < C; id++) {
            u128 m = (u128)1 << id;
            for (int k = 0; k < adj_deg[id]; k++)
                m |= (u128)1 << adj[id][k];
            self_adj_mask128[id] = m;
        }
        // owner_mask 初期化
        memset(owner_mask, 0, sizeof(owner_mask));

        rep(p, M) {
            int id = input.start_pos[p];
            pos[p] = id;
            owner[id] = p;
            level[id] = 1;
            score[p] += input.V[id];
            owner_mask[p] |= (u128)1 << id;
        }
        // reachable_mask / players_mask の初期化
        players_mask = 0;
        top_enemy = 1;
        rep(p, M) {
            rebuild_reachable_mask(p);
            players_mask |= (u128)1 << pos[p];
            if (p > 0 && score[top_enemy] < score[p])
                top_enemy = p;
        }
    }

    void copy_topology_from(const GameState &rhs) {
        N = rhs.N;
        M = rhs.M;
        U = rhs.U;
        C = rhs.C;
        memcpy(cell_x, rhs.cell_x, sizeof(cell_x));
        memcpy(cell_y, rhs.cell_y, sizeof(cell_y));
        memcpy(adj_deg, rhs.adj_deg, sizeof(adj_deg));
        memcpy(adj, rhs.adj, sizeof(adj));
        memcpy(self_adj_mask_lo, rhs.self_adj_mask_lo,
               sizeof(self_adj_mask_lo));
        memcpy(self_adj_mask_hi, rhs.self_adj_mask_hi,
               sizeof(self_adj_mask_hi));
        board_mask = rhs.board_mask;
        col0_mask = rhs.col0_mask;
        col_last_mask = rhs.col_last_mask;
        memcpy(self_adj_mask128, rhs.self_adj_mask128,
               sizeof(self_adj_mask128));
        top_enemy = rhs.top_enemy;
    }

    void copy_dynamic_from(const GameState &rhs) {
        // owner〜owner_mask
        // が連続配置されているため1回のmemcpyで全動的フィールドをコピー
        memcpy(this, &rhs, offsetof(GameState, cell_x));
    }

    void collect_reachable_owned_cells(int player, i8 reachable_ids[MAXCELLS],
                                       int &reachable_n) const {
        u64 reachable_lo = 0, reachable_hi = 0;
        i8 q[MAXCELLS];
        int qh = 0, qt = 0;
        reachable_n = 0;
        int start = pos[player];
        if (start < 64)
            reachable_lo |= (1ULL << start);
        else
            reachable_hi |= (1ULL << (start - 64));
        q[qt++] = start;
        reachable_ids[reachable_n++] = start;

        while (qh < qt) {
            int v = q[qh++];
            for (int k = 0; k < adj_deg[v]; k++) {
                int nv = adj[v][k];
                bool seen = (nv < 64) ? ((reachable_lo >> nv) & 1ULL)
                                      : ((reachable_hi >> (nv - 64)) & 1ULL);
                if (!seen && owner[nv] == player) {
                    if (nv < 64)
                        reachable_lo |= (1ULL << nv);
                    else
                        reachable_hi |= (1ULL << (nv - 64));
                    q[qt++] = nv;
                    reachable_ids[reachable_n++] = nv;
                }
            }
        }
    }

    void get_candidates(int player, CandList &cands) const {
        u128 mask = get_cand_mask(player);
        get_candidates_from_mask(mask, cands);
    }

    // 変更2: i8 dist + インラインBFS統合
    // 変更(機能): 等距離のマスもカウントするように変更
    int count_cells_closest_to_player_for_eval(int player) const {
        constexpr i8 INF_DIST = 127;
        i8 dist[MAXM][MAXCELLS];
        memset(dist, INF_DIST, sizeof(i8) * M * MAXCELLS);

        for (int p = 0; p < M; p++) {
            // collect_reachable_owned_cells をインライン化し、
            // 到達可能領土BFS + 全方向BFSを1パスで実行
            u64 visited_lo = 0, visited_hi = 0;
            i8 q[MAXCELLS];
            int qh = 0, qt = 0;

            int start = pos[p];
            if (start < 64)
                visited_lo |= (1ULL << start);
            else
                visited_hi |= (1ULL << (start - 64));
            dist[p][start] = 0;
            q[qt++] = start;

            // フェーズ1: 自領土内BFS (距離0)
            int phase1_end = qt;
            while (qh < phase1_end) {
                int v = q[qh++];
                for (int k = 0; k < adj_deg[v]; k++) {
                    int nv = adj[v][k];
                    bool seen = (nv < 64) ? ((visited_lo >> nv) & 1ULL)
                                          : ((visited_hi >> (nv - 64)) & 1ULL);
                    if (!seen && owner[nv] == p) {
                        if (nv < 64)
                            visited_lo |= (1ULL << nv);
                        else
                            visited_hi |= (1ULL << (nv - 64));
                        dist[p][nv] = 0;
                        q[qt++] = nv;
                        phase1_end = qt;
                    }
                }
            }

            // フェーズ2: 距離0セルから全方向BFS
            qh = 0;
            while (qh < qt) {
                int v = q[qh++];
                i8 nd = dist[p][v] + 1;
                for (int k = 0; k < adj_deg[v]; k++) {
                    int nv = adj[v][k];
                    if (dist[p][nv] <= nd)
                        continue;
                    dist[p][nv] = nd;
                    q[qt++] = nv;
                }
            }
        }

        int cnt = 0;
        for (int cell = 0; cell < C; cell++) {
            i8 d0 = dist[player][cell];
            if (d0 >= INF_DIST)
                continue;
            bool best = true;
            for (int p = 0; p < M; p++) {
                if (p == player)
                    continue;
                if (dist[p][cell] < d0) {
                    best = false;
                    break;
                }
            }
            if (best)
                cnt++;
        }
        return cnt;
    }

    // AI評価値を計算 (パラメータwa,wb,wc,wd指定)
    __attribute__((always_inline)) double eval_cell(int player, int cell,
                                                    double wa, double wb,
                                                    double wc,
                                                    double wd) const {
        int o = owner[cell];
        double v = input.V[cell];
        if (o == -1)
            return v * wa;
        if (o == player)
            return (level[cell] < U) ? v * wb : 0.0;
        return (level[cell] == 1) ? v * wc : v * wd;
    }

    // 対象マスcellを奪取したとき、所有者oの3x3近傍が4連結で分離するか判定
    // (外側でつながっている可能性は無視する近似判定)
    bool would_split_enemy_local(int cell) const {
        int o = owner[cell];
        if (o < 0)
            return false;

        u8 mask = 0;
        for (int k = 0; k < 8; k++) {
            int id = GLOBAL_LOCAL8_NEIGHBOR_ID[N][cell][k];
            if (id >= 0 && owner[id] == o)
                mask |= (u8)(1u << k);
        }
        return LocalSplit3x3::SPLIT_LUT[mask];
    }

    // 自手(プレイヤー0)の評価値を計算
    // no_enemy_prob[cell] を参照して確率ベースのボーナスを適用
    __attribute__((always_inline)) double eval_self(int cell) const {
        int o = owner[cell];
        double v = input.V[cell];
        double e;
        bool is_top = (o > 0 && o == top_enemy);
        // p_enemy: このセルに敵が一人以上来る確率
        float no_enemy = no_enemy_prob[cell];
        float p_enemy = 1.0f - no_enemy;
        if (o == -1) {
            e = v * CURRENT_SELF_EVAL_EMPTY;
            e *= no_enemy;
        } else if (o == 0) {
            int lv = level[cell];
            e = v * CURRENT_SELF_EVAL_OWN_LEVEL[lv];
            e *= (1.0f + p_enemy);
        } else {
            int lv = level[cell];
            e = v * (is_top ? CURRENT_SELF_EVAL_ENEMY_TOP_LEVEL[lv]
                            : CURRENT_SELF_EVAL_ENEMY_NOT_TOP_LEVEL[lv]);
            e *= no_enemy;
        }
        return e;
    }

    // 変更1: プレイアウト用 (score/enemy順位更新付き +
    // ペアワイズ衝突)
    void apply_moves_fast(i8 to[MAXM]) {
        if (M == 2) {
            if (to[0] == to[1]) {
                if (owner[to[0]] == 0) {
                    // 自陣
                    players_mask = 0;
                    int cell = to[0];
                    int v = input.V[cell];
                    pos[0] = cell;
                    if (level[cell] < U) {
                        level[cell]++;
                        score[0] += v;
                    }
                    players_mask |= (u128)1 << cell;
                    players_mask |= (u128)1 << pos[1];
                } else if (owner[to[0]] == 1) {
                    // 敵陣
                    players_mask = 0;
                    int cell = to[1];
                    int v = input.V[cell];
                    pos[1] = cell;
                    if (level[cell] < U) {
                        level[cell]++;
                        score[1] += v;
                    }
                    players_mask |= (u128)1 << cell;
                    players_mask |= (u128)1 << pos[0];
                } else {
                    // 空きマス
                    // 何も起こらないが、players_mask は現位置を維持
                }
            } else {
                u8 dirty = 0;
                players_mask = 0;
                for (int p = 0; p < M; p++) {
                    int cell = to[p];
                    int o = owner[cell];
                    int v = input.V[cell];
                    if (o == p) {
                        // 自陣への移動
                        pos[p] = cell;
                        if (level[cell] < U) {
                            level[cell]++;
                            score[p] += v;
                        }
                        players_mask |= (u128)1 << cell;
                    } else if (o == -1) {
                        // 空きマスへの移動
                        pos[p] = cell;
                        owner[cell] = p;
                        level[cell] = 1;
                        score[p] += v;
                        u128 m = (u128)1 << cell;
                        players_mask |= m;
                        owner_mask[p] |= m;
                        dirty |= (u8)(1u << p);
                    } else if (level[cell] > 1) {
                        // 持ち主は維持
                        level[cell]--;
                        score[o] -= v;
                        players_mask |= (u128)1 << pos[p];
                    } else {
                        // 奪取
                        pos[p] = cell;
                        owner[cell] = p;
                        level[cell] = 1;
                        score[p] += v;
                        score[o] -= v;
                        u128 m = (u128)1 << cell;
                        players_mask |= m;
                        owner_mask[p] |= m;
                        owner_mask[o] &= ~m;
                        dirty |= (u8)((1u << p) | (1u << o));
                    }
                }
                // owner_mask が変化したプレイヤーのみ reachable_mask を再計算
                while (dirty) {
                    int p = __builtin_ctz(dirty);
                    dirty &= dirty - 1;
                    rebuild_reachable_mask(p);
                }
            }
        } else {
            players_mask = 0;
            // player_cnt は to[p] で示されるM個のセルしか使わないため、
            // 100要素ゼロ初期化の代わりにM要素だけピンポイントでゼロ化する
            int player_cnt[MAXCELLS];
            for (int p = 0; p < M; p++)
                player_cnt[to[p]] = 0;
            // dirty: ビット p が立っていれば owner_mask[p] が変化した
            u8 dirty = 0;
            i8 not_confirm[MAXM];
            int not_confirm_n = 0;
            for (int p = 0; p < M; p++) {
                int cell = to[p];
                int o = owner[cell];
                int v = input.V[cell];
                // 一週目で自陣への移動を確定させる
                if (o == p) {
                    player_cnt[cell]++;
                    pos[p] = cell;
                    if (level[cell] < U) {
                        level[cell]++;
                        score[p] += v;
                    }
                    players_mask |= (u128)1 << cell;
                } else {
                    player_cnt[cell]++;
                    not_confirm[not_confirm_n++] = p;
                }
            }
            for (int i = 0; i < not_confirm_n; i++) {
                int p = not_confirm[i];
                int cell = to[p];
                int o = owner[cell];
                int v = input.V[cell];
                // 二周目で全ての行動を反映
                if (player_cnt[cell] == 1) {
                    // 移動可能
                    if (o == -1) {
                        // 空きマス
                        u128 m = (u128)1 << cell;
                        pos[p] = cell;
                        owner[cell] = p;
                        level[cell] = 1;
                        score[p] += v;
                        owner_mask[p] |= m;
                        dirty |= (u8)(1u << p);
                        players_mask |= m;
                    } else if (level[cell] > 1) {
                        // 持ち主は維持
                        level[cell]--;
                        score[o] -= v;
                        players_mask |= (u128)1 << pos[p];
                    } else {
                        // 奪取
                        u128 m = (u128)1 << cell;
                        pos[p] = cell;
                        owner[cell] = p;
                        level[cell] = 1;
                        score[p] += v;
                        score[o] -= v;
                        owner_mask[p] |= m;
                        owner_mask[o] &= ~m;
                        dirty |= (u8)((1u << p) | (1u << o));
                        players_mask |= m;
                    }
                } else {
                    // 移動不可
                    players_mask |= (u128)1 << pos[p];
                }
            }
            // owner_mask が変化したプレイヤーのみ reachable_mask を再計算
            while (dirty) {
                int p = __builtin_ctz(dirty);
                dirty &= dirty - 1;
                rebuild_reachable_mask(p);
            }
            int max_score = score[1];
            top_enemy = 1;
            for (int p = 2; p < M; p++) {
                if (max_score < score[p]) {
                    max_score = score[p];
                    top_enemy = p;
                }
            }
        }
    }

    // apply_moves_fast の rebuild_reachable_mask を省略したバージョン
    // (最後のdepthステップ用: 以降 get_cand_mask を呼ばないのでrebuild不要)
    void apply_moves_fast_no_rebuild(i8 to[MAXM]) {
        if (M == 2) {
            if (to[0] == to[1]) {
                if (owner[to[0]] == 0) {
                    // 自陣
                    players_mask = 0;
                    int cell = to[0];
                    int v = input.V[cell];
                    pos[0] = cell;
                    if (level[cell] < U) {
                        level[cell]++;
                        score[0] += v;
                    }
                    players_mask |= (u128)1 << cell;
                    players_mask |= (u128)1 << pos[1];
                } else if (owner[to[0]] == 1) {
                    // 敵陣
                    players_mask = 0;
                    int cell = to[1];
                    int v = input.V[cell];
                    pos[1] = cell;
                    if (level[cell] < U) {
                        level[cell]++;
                        score[1] += v;
                    }
                    players_mask |= (u128)1 << cell;
                    players_mask |= (u128)1 << pos[0];
                } else {
                    // 空きマス
                    // 何も起こらないが、players_mask は現位置を維持
                }
            } else {
                players_mask = 0;
                for (int p = 0; p < M; p++) {
                    int cell = to[p];
                    int o = owner[cell];
                    int v = input.V[cell];
                    if (o == p) {
                        // 自陣への移動
                        pos[p] = cell;
                        if (level[cell] < U) {
                            level[cell]++;
                            score[p] += v;
                        }
                        players_mask |= (u128)1 << cell;
                    } else if (o == -1) {
                        // 空きマスへの移動
                        pos[p] = cell;
                        owner[cell] = p;
                        level[cell] = 1;
                        score[p] += v;
                        u128 m = (u128)1 << cell;
                        players_mask |= m;
                        owner_mask[p] |= m;
                    } else if (level[cell] > 1) {
                        // 持ち主は維持
                        level[cell]--;
                        score[o] -= v;
                        players_mask |= (u128)1 << pos[p];
                    } else {
                        // 奪取
                        pos[p] = cell;
                        owner[cell] = p;
                        level[cell] = 1;
                        score[p] += v;
                        score[o] -= v;
                        u128 m = (u128)1 << cell;
                        players_mask |= m;
                        owner_mask[p] |= m;
                        owner_mask[o] &= ~m;
                    }
                }
            }
        } else {
            players_mask = 0;
            int player_cnt[MAXCELLS];
            for (int p = 0; p < M; p++)
                player_cnt[to[p]] = 0;
            for (int p = 0; p < M; p++) {
                int cell = to[p];
                int o = owner[cell];
                int v = input.V[cell];
                if (o == p) {
                    player_cnt[cell]++;
                    pos[p] = cell;
                    if (level[cell] < U) {
                        level[cell]++;
                        score[p] += v;
                    }
                    players_mask |= (u128)1 << cell;
                } else {
                    player_cnt[cell]++;
                }
            }
            for (int p = 0; p < M; p++) {
                int cell = to[p];
                int o = owner[cell];
                int v = input.V[cell];
                if (o == p)
                    continue;
                if (player_cnt[cell] == 1) {
                    if (o == -1) {
                        u128 m = (u128)1 << cell;
                        pos[p] = cell;
                        owner[cell] = p;
                        level[cell] = 1;
                        score[p] += v;
                        owner_mask[p] |= m;
                        players_mask |= m;
                    } else if (level[cell] > 1) {
                        level[cell]--;
                        score[o] -= v;
                        players_mask |= (u128)1 << pos[p];
                    } else {
                        u128 m = (u128)1 << cell;
                        pos[p] = cell;
                        owner[cell] = p;
                        level[cell] = 1;
                        score[p] += v;
                        score[o] -= v;
                        owner_mask[p] |= m;
                        owner_mask[o] &= ~m;
                        players_mask |= m;
                    }
                } else {
                    players_mask |= (u128)1 << pos[p];
                }
            }
        }
        // rebuild_reachable_mask は省略
        for (int p = 1; p < M; p++) {
            if (score[top_enemy] < score[p])
                top_enemy = p;
        }
    }

    void calc_scores(i32 scores[MAXM]) const {
        memset(scores, 0, sizeof(i32) * MAXM);
        for (int cell = 0; cell < C; cell++) {
            int o = owner[cell];
            if (o < 0)
                continue;
            scores[o] += input.V[cell] * level[cell];
        }
    }
};

// ==========================================
// セル種別情報 (尤度O(1)計算用の前処理)
// ==========================================
struct CellTypeInfo {
    int max_V[4];
    int max_count[4];
    bool type_exists[4];
    int target_type;
    int target_V;
    bool target_is_max_in_type;
    int n_cands;
};

static CellTypeInfo precompute_cell_info(int player, int target_cell,
                                         const GameState &state,
                                         const CandList &cands) {
    CellTypeInfo info;
    info.n_cands = cands.n;
    for (int t = 0; t < 4; t++) {
        info.max_V[t] = 0;
        info.max_count[t] = 0;
        info.type_exists[t] = false;
    }
    info.target_V = input.V[target_cell];
    info.target_is_max_in_type = false;

    {
        int o = state.owner[target_cell];
        if (state.U == 1) {
            if (o == -1)
                info.target_type = 0;
            else if (o == player)
                info.target_type = -1;
            else
                info.target_type = 2;
        } else {
            if (o == -1)
                info.target_type = 0;
            else if (o == player)
                info.target_type =
                    (state.level[target_cell] < state.U) ? 1 : -1;
            else
                info.target_type = (state.level[target_cell] == 1) ? 2 : 3;
        }
    }

    for (int k = 0; k < cands.n; k++) {
        int cell = cands.ids[k];
        int o = state.owner[cell];
        int type;
        if (state.U == 1) {
            if (o == -1)
                type = 0;
            else if (o == player)
                continue;
            else
                type = 2;
        } else {
            if (o == -1)
                type = 0;
            else if (o == player) {
                if (state.level[cell] < state.U)
                    type = 1;
                else
                    continue;
            } else {
                type = (state.level[cell] == 1) ? 2 : 3;
            }
        }

        int v = input.V[cell];
        info.type_exists[type] = true;
        if (v > info.max_V[type]) {
            info.max_V[type] = v;
            info.max_count[type] = 1;
        } else if (v == info.max_V[type]) {
            info.max_count[type]++;
        }
    }

    if (info.target_type >= 0) {
        info.target_is_max_in_type =
            (info.target_V == info.max_V[info.target_type]);
    }

    return info;
}

static inline double compute_likelihood_fast(const CellTypeInfo &info,
                                             int wa_int, int wb_int, int wc_int,
                                             int wd_int, double eps) {
    int w_int[4] = {wa_int, wb_int, wc_int, wd_int};

    i64 max_eval = 0;
    int greedy_count = 0;

    for (int t = 0; t < 4; t++) {
        if (!info.type_exists[t])
            continue;
        i64 eval = (i64)info.max_V[t] * w_int[t];
        if (eval > max_eval) {
            max_eval = eval;
            greedy_count = info.max_count[t];
        } else if (eval == max_eval) {
            greedy_count += info.max_count[t];
        }
    }

    if (greedy_count == 0) {
        return 1.0 / info.n_cands;
    }

    bool target_is_greedy = false;
    if (info.target_type >= 0 && info.target_is_max_in_type) {
        i64 target_eval = (i64)info.target_V * w_int[info.target_type];
        target_is_greedy = (target_eval == max_eval);
    }

    return target_is_greedy ? (1.0 - eps) / greedy_count + eps / info.n_cands
                            : eps / info.n_cands;
}

// ==========================================
// 粒子フィルター推定器 (wa,wb,wc,wd: 4次元粒子フィルター, eps:
// 1次元離散グリッド)
// ==========================================
struct ParticleFilterEstimator {
    // === 粒子フィルター (wa, wb, wc, wd) — SoAレイアウト ===
    // U=1のときwb,wdは無意味(自陣はレベルが常にU、他陣もレベルが常に1)なので2次元に縮約
    double pf_wa[MAXM][N_PARTICLES];
    double pf_wb[MAXM][N_PARTICLES];
    double pf_wc[MAXM][N_PARTICLES];
    double pf_wd[MAXM][N_PARTICLES];
    double pf_log_w[MAXM][N_PARTICLES]; // 対数重み
    bool pf_uniform[MAXM];              // true=均等重み（高速サンプリング可）
    int U_;                             // レベル上限 (U=1ならwb,wd推定スキップ)

    // サンプリング用キャッシュ（Alias法, 不均等時のみ使用）
    double pf_prob[MAXM][N_PARTICLES];
    int pf_alias[MAXM][N_PARTICLES];
    bool pf_cache_valid[MAXM];

    // === 1次元eps推定（現行方式を維持） ===
    double log_e[MAXM][N_EPS_F];
    double cum_e[MAXM][N_EPS_F];

    void init(int M) {
        init_eps_table();
        U_ = input.U;
        double lp_e = -log((double)N_EPS_F);
        for (int p = 1; p < M; p++) {
            // 粒子を一様乱数で初期化
            for (int i = 0; i < N_PARTICLES; i++) {
                pf_wa[p][i] = rng.randomRangeDouble(0.3, 1.0);
                // U=1: wb,wdは使われないので固定値
                pf_wb[p][i] = (U_ == 1) ? 0.5 : rng.randomRangeDouble(0.3, 1.0);
                pf_wc[p][i] = rng.randomRangeDouble(0.3, 1.0);
                pf_wd[p][i] = (U_ == 1) ? 0.5 : rng.randomRangeDouble(0.3, 1.0);
                pf_log_w[p][i] = 0.0;
            }
            pf_uniform[p] = true;
            pf_cache_valid[p] = false;
            // eps離散グリッド
            fill(log_e[p], log_e[p] + N_EPS_F, lp_e);
            rebuild_eps_cdf(p);
        }
    }

    void rebuild_eps_cdf(int p) {
        double mx = *max_element(log_e[p], log_e[p] + N_EPS_F);
        double s = 0;
        double exps[N_EPS_F];
        for (int i = 0; i < N_EPS_F; i++) {
            exps[i] = exp(log_e[p][i] - mx);
            s += exps[i];
        }
        double inv_s = 1.0 / s;
        double c = 0;
        for (int i = 0; i < N_EPS_F; i++) {
            c += exps[i] * inv_s;
            cum_e[p][i] = c;
        }
        cum_e[p][N_EPS_F - 1] = 1.0;
    }

    void rebuild_pf_cache(int p) {
        if (pf_uniform[p]) {
            pf_cache_valid[p] = true;
            return;
        }
        double mx = *max_element(pf_log_w[p], pf_log_w[p] + N_PARTICLES);
        double wbuf[N_PARTICLES];
        double s = 0.0;
        for (int i = 0; i < N_PARTICLES; i++) {
            double w = exp(pf_log_w[p][i] - mx);
            wbuf[i] = w;
            s += w;
        }

        double scale = (double)N_PARTICLES / s;
        double scaled[N_PARTICLES];
        int small[N_PARTICLES], large[N_PARTICLES];
        int ns = 0, nl = 0;
        for (int i = 0; i < N_PARTICLES; i++) {
            scaled[i] = wbuf[i] * scale;
            if (scaled[i] < 1.0)
                small[ns++] = i;
            else
                large[nl++] = i;
        }

        while (ns > 0 && nl > 0) {
            int sidx = small[--ns];
            int lidx = large[--nl];
            pf_prob[p][sidx] = scaled[sidx];
            pf_alias[p][sidx] = lidx;
            scaled[lidx] = (scaled[lidx] + scaled[sidx]) - 1.0;
            if (scaled[lidx] < 1.0)
                small[ns++] = lidx;
            else
                large[nl++] = lidx;
        }
        while (nl > 0) {
            int idx = large[--nl];
            pf_prob[p][idx] = 1.0;
            pf_alias[p][idx] = idx;
        }
        while (ns > 0) {
            int idx = small[--ns];
            pf_prob[p][idx] = 1.0;
            pf_alias[p][idx] = idx;
        }
        pf_cache_valid[p] = true;
    }

    int sample_e_idx(int p) const {
        double r = rng.randomDouble();
        int idx =
            (int)(upper_bound(cum_e[p], cum_e[p] + N_EPS_F, r) - cum_e[p]);
        return min(idx, N_EPS_F - 1);
    }

    // epsの期待値を計算
    double eps_mean(int p) const {
        double mx = *max_element(log_e[p], log_e[p] + N_EPS_F);
        double s = 0, wsum = 0;
        for (int i = 0; i < N_EPS_F; i++) {
            double w = exp(log_e[p][i] - mx);
            s += w;
            wsum += w * EPS_DBL_F[i];
        }
        return wsum / s;
    }

    // 粒子をサンプル (インデックスを返す)
    __attribute__((always_inline)) int sample_particle_idx(int p) {
        if (pf_uniform[p])
            return rng.random32(N_PARTICLES);
        if (!pf_cache_valid[p])
            rebuild_pf_cache(p);
        int idx = rng.random32(N_PARTICLES);
        return (rng.randomDouble() < pf_prob[p][idx]) ? idx : pf_alias[p][idx];
    }

    static void normalize_log(double *arr, int n) {
        double mx = *max_element(arr, arr + n);
        double s = 0;
        for (int i = 0; i < n; i++)
            s += exp(arr[i] - mx);
        double ln = mx + log(s);
        for (int i = 0; i < n; i++)
            arr[i] -= ln;
    }

    void liu_west_resample(int p) {
        // 1. 正規化された重みを計算
        double mx = *max_element(pf_log_w[p], pf_log_w[p] + N_PARTICLES);
        double weights[N_PARTICLES];
        double sum_w = 0;
        for (int i = 0; i < N_PARTICLES; i++) {
            weights[i] = exp(pf_log_w[p][i] - mx);
            sum_w += weights[i];
        }
        double inv_sum = 1.0 / sum_w;
        for (int i = 0; i < N_PARTICLES; i++)
            weights[i] *= inv_sum;

        // 2. 加重平均・加重標準偏差（U=1なら2次元(wa,wc)、それ以外は4次元）
        const bool skip_wb_wd = (U_ == 1);
        double mean[4] = {}, var[4] = {};
        for (int i = 0; i < N_PARTICLES; i++) {
            double w = weights[i];
            mean[0] += w * pf_wa[p][i];
            if (!skip_wb_wd)
                mean[1] += w * pf_wb[p][i];
            mean[2] += w * pf_wc[p][i];
            if (!skip_wb_wd)
                mean[3] += w * pf_wd[p][i];
        }
        for (int i = 0; i < N_PARTICLES; i++) {
            double w = weights[i];
            double d0 = pf_wa[p][i] - mean[0];
            var[0] += w * d0 * d0;
            if (!skip_wb_wd) {
                double d1 = pf_wb[p][i] - mean[1];
                var[1] += w * d1 * d1;
            }
            double d2 = pf_wc[p][i] - mean[2];
            var[2] += w * d2 * d2;
            if (!skip_wb_wd) {
                double d3 = pf_wd[p][i] - mean[3];
                var[3] += w * d3 * d3;
            }
        }
        double sd[4];
        sd[0] = sqrt(var[0]);
        sd[1] = skip_wb_wd ? 0.0 : sqrt(var[1]);
        sd[2] = sqrt(var[2]);
        sd[3] = skip_wb_wd ? 0.0 : sqrt(var[3]);

        // 3. 系統的リサンプリング
        double cumsum[N_PARTICLES];
        cumsum[0] = weights[0];
        for (int i = 1; i < N_PARTICLES; i++)
            cumsum[i] = cumsum[i - 1] + weights[i];

        // リサンプリング結果を一時領域に格納
        double new_wa[N_PARTICLES], new_wb[N_PARTICLES];
        double new_wc[N_PARTICLES], new_wd[N_PARTICLES];

        double u0 = rng.randomDouble() / N_PARTICLES;
        int j = 0;
        for (int i = 0; i < N_PARTICLES; i++) {
            double u = u0 + (double)i / N_PARTICLES;
            while (j < N_PARTICLES - 1 && cumsum[j] < u)
                j++;
            new_wa[i] = pf_wa[p][j];
            new_wb[i] = pf_wb[p][j];
            new_wc[i] = pf_wc[p][j];
            new_wd[i] = pf_wd[p][j];
        }

        // 4. Liu-West カーネル: 収縮 + ガウスノイズ
        for (int i = 0; i < N_PARTICLES; i++) {
            new_wa[i] = LW_A * new_wa[i] + (1.0 - LW_A) * mean[0] +
                        LW_H * sd[0] * rng.randomGaussian();
            if (!skip_wb_wd) {
                new_wb[i] = LW_A * new_wb[i] + (1.0 - LW_A) * mean[1] +
                            LW_H * sd[1] * rng.randomGaussian();
            }
            new_wc[i] = LW_A * new_wc[i] + (1.0 - LW_A) * mean[2] +
                        LW_H * sd[2] * rng.randomGaussian();
            if (!skip_wb_wd) {
                new_wd[i] = LW_A * new_wd[i] + (1.0 - LW_A) * mean[3] +
                            LW_H * sd[3] * rng.randomGaussian();
            }
            // クリッピング [0.3, 1.0]
            new_wa[i] = clamp(new_wa[i], 0.3, 1.0);
            if (!skip_wb_wd)
                new_wb[i] = clamp(new_wb[i], 0.3, 1.0);
            new_wc[i] = clamp(new_wc[i], 0.3, 1.0);
            if (!skip_wb_wd)
                new_wd[i] = clamp(new_wd[i], 0.3, 1.0);
        }

        // 5. コピーバック、重みを均等にリセット
        memcpy(pf_wa[p], new_wa, sizeof(double) * N_PARTICLES);
        memcpy(pf_wb[p], new_wb, sizeof(double) * N_PARTICLES);
        memcpy(pf_wc[p], new_wc, sizeof(double) * N_PARTICLES);
        memcpy(pf_wd[p], new_wd, sizeof(double) * N_PARTICLES);
        fill(pf_log_w[p], pf_log_w[p] + N_PARTICLES, 0.0);
        pf_uniform[p] = true;
        pf_cache_valid[p] = false;
    }

    void update(int p, int target_cell, const GameState &state) {
        CandList cands;
        state.get_candidates(p, cands);
        if (cands.n <= 1)
            return;

        CellTypeInfo info = precompute_cell_info(p, target_cell, state, cands);

        bool any_type = false;
        for (int t = 0; t < 4; t++)
            if (info.type_exists[t]) {
                any_type = true;
                break;
            }
        if (!any_type)
            return;

        // epsの期待値を計算（粒子重み更新用）
        double eps_exp = eps_mean(p);

        // 全粒子の整数w値を前計算
        int wa_int[N_PARTICLES], wb_int[N_PARTICLES], wc_int[N_PARTICLES];
        for (int i = 0; i < N_PARTICLES; i++) {
            wa_int[i] = (int)lround(pf_wa[p][i] * 1000);
            // U=1: wb,wdは使われないので0固定
            wb_int[i] = (U_ == 1) ? 0 : (int)lround(pf_wb[p][i] * 1000);
            wc_int[i] = (int)lround(pf_wc[p][i] * 1000);
        }

        if (U_ == 1) {
            // U=1ではwb/type1, wd/type3は到達しないので計算対象から外す
            for (int i = 0; i < N_PARTICLES; i++) {
                double lik = compute_likelihood_fast(info, wa_int[i], wb_int[i],
                                                     wc_int[i], 0, eps_exp);
                pf_log_w[p][i] += log(lik);
            }
            pf_uniform[p] = false;
            pf_cache_valid[p] = false;

            for (int ei = 0; ei < N_EPS_F; ei++) {
                double sum_lik = 0;
                for (int s = 0; s < N_EPS_SAMPLES; s++) {
                    int idx = sample_particle_idx(p);
                    sum_lik +=
                        compute_likelihood_fast(info, wa_int[idx], wb_int[idx],
                                                wc_int[idx], 0, EPS_DBL_F[ei]);
                }
                log_e[p][ei] += log(sum_lik / N_EPS_SAMPLES);
            }
        } else {
            int wd_int[N_PARTICLES];
            for (int i = 0; i < N_PARTICLES; i++)
                wd_int[i] = (int)lround(pf_wd[p][i] * 1000);

            for (int i = 0; i < N_PARTICLES; i++) {
                double lik = compute_likelihood_fast(
                    info, wa_int[i], wb_int[i], wc_int[i], wd_int[i], eps_exp);
                pf_log_w[p][i] += log(lik);
            }
            pf_uniform[p] = false;
            pf_cache_valid[p] = false;

            for (int ei = 0; ei < N_EPS_F; ei++) {
                double sum_lik = 0;
                for (int s = 0; s < N_EPS_SAMPLES; s++) {
                    int idx = sample_particle_idx(p);
                    sum_lik += compute_likelihood_fast(
                        info, wa_int[idx], wb_int[idx], wc_int[idx],
                        wd_int[idx], EPS_DBL_F[ei]);
                }
                log_e[p][ei] += log(sum_lik / N_EPS_SAMPLES);
            }
        }

        normalize_log(log_e[p], N_EPS_F);
        rebuild_eps_cdf(p);

        // --- ESS計算・リサンプリング判定 ---
        double mx = *max_element(pf_log_w[p], pf_log_w[p] + N_PARTICLES);
        double sum_w = 0, sum_sq = 0;
        for (int i = 0; i < N_PARTICLES; i++) {
            double w = exp(pf_log_w[p][i] - mx);
            sum_w += w;
            sum_sq += w * w;
        }
        double ess = (sum_w * sum_w) / sum_sq;
        if (ess < N_PARTICLES * ESS_THRESHOLD_RATIO) {
            liu_west_resample(p);
        }
    }

    void sample_params(int p, double &wa, double &wb, double &wc, double &wd,
                       double &eps) {
        int idx = sample_particle_idx(p);
        wa = pf_wa[p][idx];
        wb = pf_wb[p][idx];
        wc = pf_wc[p][idx];
        wd = pf_wd[p][idx];
        eps = EPS_DBL_F[sample_e_idx(p)];
    }

    void map_params(int p, double &wa, double &wb, double &wc, double &wd,
                    double &eps) const {
        // 加重平均を返す
        double mx = *max_element(pf_log_w[p], pf_log_w[p] + N_PARTICLES);
        double s = 0;
        wa = wb = wc = wd = 0;
        for (int i = 0; i < N_PARTICLES; i++) {
            double w = exp(pf_log_w[p][i] - mx);
            s += w;
            wa += w * pf_wa[p][i];
            wb += w * pf_wb[p][i];
            wc += w * pf_wc[p][i];
            wd += w * pf_wd[p][i];
        }
        wa /= s;
        wb /= s;
        wc /= s;
        wd /= s;
        eps = EPS_DBL_F[max_element(log_e[p], log_e[p] + N_EPS_F) - log_e[p]];
    }
};

// ==========================================
// AI手選択
// ==========================================
// u128マスクからランダムにセットビットを1つ選ぶ
// n = popcount128(mask) を呼び出し元から受け取ることで重複計算を回避
static inline int random_bit_from_mask128(u128 mask) {
    int k = rng.random32(popcount128(mask));
    for (int i = 0; i < k; i++)
        mask &= mask - 1;
    return ctz128(mask);
}

// u128マスクからCandList展開なしでAI手を選択
static inline int simulate_ai_move(int p, const GameState &state, int wa_int,
                                   int wb_int, int wc_int, int wd_int,
                                   double eps) {
    u128 mask = state.get_cand_mask(p);

    if (rng.randomDouble() < eps)
        return random_bit_from_mask128(mask);

    int max_eval_int = INT_MIN, best_cell = -1, best_n = 0;

    u128 m = mask;
    while (m) {
        int cell = ctz128(m);
        m &= m - 1;

        int o = state.owner[cell];
        int v = input.V[cell];

        int e_int;
        if (o == -1)
            e_int = v * wa_int;
        else if (o == p)
            e_int = (state.level[cell] < state.U) ? v * wb_int : 0;
        else
            e_int = (state.level[cell] == 1) ? v * wc_int : v * wd_int;
        if (e_int > max_eval_int) {
            max_eval_int = e_int;
            best_cell = cell;
            best_n = 1;
        } else if (e_int == max_eval_int) {
            if (rng.random32(++best_n) == 0)
                best_cell = cell;
        }
    }
    return best_cell;
}

// U=1専用: レベル分岐を削った敵手シミュレーション
static inline int simulate_ai_move_u1(int p, const GameState &state, int wa_int,
                                      int wc_int, double eps) {
    u128 mask = state.get_cand_mask(p);

    if (rng.randomDouble() < eps)
        return random_bit_from_mask128(mask);

    int max_eval_int = INT_MIN, best_cell = -1, best_n = 0;

    u128 m = mask;
    while (m) {
        int cell = ctz128(m);
        m &= m - 1;

        int o = state.owner[cell];
        int v = input.V[cell];
        int e_int = (o == -1) ? (v * wa_int) : ((o == p) ? 0 : (v * wc_int));
        if (e_int > max_eval_int) {
            max_eval_int = e_int;
            best_cell = cell;
            best_n = 1;
        } else if (e_int == max_eval_int) {
            if (rng.random32(++best_n) == 0)
                best_cell = cell;
        }
    }
    return best_cell;
}

// ==========================================
// 敵手シミュレーション + no_enemy_prob 更新
// ==========================================
// mask を1回走査して:
//   - ランダム来ない確率(1-eps/n)をその場でno_enemy_probに掛ける
//     (my_mask外のセルに掛けても参照されないので無害)
//   - greedy選択(reservoir sampling)
// 走査後: best_cell 1セルにgreedy来ない確率の追加補正をO(1)で適用
//   来ない確率: 1 - eps/n - (1-eps)/best_n
//   既にrand_no=(1-eps/n)を掛けてあるので追加乗数:
//   1 - (1-eps)/best_n / rand_no
static inline int simulate_ai_move_and_update_prob(int p, GameState &state,
                                                   int wa_int, int wb_int,
                                                   int wc_int, int wd_int,
                                                   double eps) {
    u128 mask = state.get_cand_mask(p);
    int n = popcount128(mask);
    if (n == 0)
        return state.pos[p];
    int k = -1;
    int random_cell = -1;
    // 返す手: eps の確率でランダム、1-eps の確率で best_cell
    if (rng.randomDouble() < eps)
        k = rng.random32(n);

    float rand_no = 1.0f - (float)(eps / n);
    int max_eval_int = INT_MIN, best_cell = -1, best_n = 0;
    u128 m = mask;
    rep(i, n) {
        int cell = ctz128(m);
        if (i == k)
            random_cell = cell;
        m &= m - 1;
        state.no_enemy_prob[cell] *= rand_no; // ランダム来ない確率を即時適用
        int o = state.owner[cell];
        int v = input.V[cell];
        int e_int;
        if (o == -1)
            e_int = v * wa_int;
        else if (o == p)
            e_int = (state.level[cell] < state.U) ? v * wb_int : 0;
        else
            e_int = (state.level[cell] == 1) ? v * wc_int : v * wd_int;
        if (e_int > max_eval_int) {
            max_eval_int = e_int;
            best_cell = cell;
            best_n = 1;
        } else if (e_int == max_eval_int) {
            if (rng.random32(++best_n) == 0)
                best_cell = cell;
        }
    }
    // 走査後: best_cell に貪欲来ない確率をO(1)で適用
    // 既に rand_no=(1-eps/n) を掛けてあるため、追加乗数は
    // (1 - eps/n - (1-eps)/best_n) / (1 - eps/n)
    if (best_cell >= 0) {
        float greedy_no =
            1.0f - (float)(eps / n) - (float)((1.0 - eps) / best_n);
        float adjust = greedy_no / rand_no;
        state.no_enemy_prob[best_cell] *= adjust;
    }
    if (k >= 0)
        return random_cell;
    else
        return best_cell;
}

// U=1専用版
static inline int simulate_ai_move_u1_and_update_prob(int p, GameState &state,
                                                      int wa_int, int wc_int,
                                                      double eps) {
    u128 mask = state.get_cand_mask(p);
    int n = popcount128(mask);
    if (n == 0)
        return state.pos[p];
    int k = -1;
    int random_cell = -1;
    if (rng.randomDouble() < eps)
        k = rng.random32(n);

    float rand_no = 1.0f - (float)(eps / n);
    int max_eval_int = INT_MIN, best_cell = -1, best_n = 0;
    u128 m = mask;
    rep(i, n) {
        int cell = ctz128(m);
        if (i == k)
            random_cell = cell;
        m &= m - 1;
        state.no_enemy_prob[cell] *= rand_no;
        int o = state.owner[cell];
        int v = input.V[cell];
        int e_int = (o == -1) ? (v * wa_int) : ((o == p) ? 0 : (v * wc_int));
        if (e_int > max_eval_int) {
            max_eval_int = e_int;
            best_cell = cell;
            best_n = 1;
        } else if (e_int == max_eval_int) {
            if (rng.random32(++best_n) == 0)
                best_cell = cell;
        }
    }

    if (best_cell >= 0) {
        float greedy_no =
            1.0f - (float)(eps / n) - (float)((1.0 - eps) / best_n);
        float adjust = greedy_no / rand_no;
        state.no_enemy_prob[best_cell] *= adjust;
    }

    if (k >= 0)
        return random_cell;
    else
        return best_cell;
}

// ==========================================
// UCBルート選択
// 強化版: 正規化 + UCB-Tuned + Empirical Bernstein半径
// ==========================================
static inline int select_root_ucb(const double *total_score,
                                  const double *total_score_sq,
                                  const int *count, int n, int total_playouts) {
    for (int i = 0; i < n; i++)
        if (count[i] == 0)
            return i;

    // 全候補の平均値のmean/stdを計算して正規化
    double global_mean = 0;
    int active = 0;
    for (int i = 0; i < n; i++) {
        if (count[i] > 0) {
            global_mean += total_score[i] / count[i];
            active++;
        }
    }
    global_mean /= active;

    double global_var = 0;
    for (int i = 0; i < n; i++) {
        if (count[i] > 0) {
            double d = total_score[i] / count[i] - global_mean;
            global_var += d * d;
        }
    }
    global_var /= active;
    double global_std = sqrt(global_var);
    if (global_std < 1e-12)
        global_std = 1.0;

    double inv_std = 1.0 / global_std;
    double inv_var = inv_std * inv_std;
    // 正規化後の報酬レンジ想定。EBの線形項(3R logN / n)のR。
    constexpr double EB_RANGE = 3.0;

    double logN = log((double)total_playouts);
    int best = 0;
    double best_val = -1e100;
    for (int i = 0; i < n; i++) {
        double mean = total_score[i] / count[i];
        double norm_mean = (mean - global_mean) * inv_std;

        double variance = total_score_sq[i] / count[i] - mean * mean;
        if (variance < 0)
            variance = 0;
        double norm_var = variance * inv_var;
        // UCB-Tunedの分散上限キャップ(0.25)を適用
        double tuned_var = min(0.25, norm_var);

        // Empirical Bernstein + tuned variance
        double n_inv = 1.0 / count[i];
        double eb_radius = sqrt(2.0 * tuned_var * logN * n_inv) +
                           3.0 * EB_RANGE * logN * n_inv;
        double val = norm_mean + CURRENT_UCB_EXPLORATION_C * eb_radius;
        if (val > best_val) {
            best_val = val;
            best = i;
        }
    }
    return best;
}

// ==========================================
// Solver
// ==========================================
class Solver {
    static constexpr int MAX_SHARED_PLAYOUT_PARAMS = 100000;
    GameState state;
    ParticleFilterEstimator estimator;
    int prev_to[MAXM];
    GameState prev_state;
    GameState sim; // プレイアウト用 (毎回生成せず再利用)

    // (enemy_top_cells / enemy_target_mask / enemy_cand_mask は削除)
    // no_enemy_prob は sim.no_enemy_prob として GameState に持つ

    // ターンごとの時間配分 (事前計算)
    double turn_weight[100];
    double turn_weight_suffix[101]; // suffix sum (index Tは0)

    // 各候補手の「プレイアウト回数の区間」で共通利用するパラメータ集合
    struct SharedPlayoutParams {
        array<double, MAXM> wa{};
        array<double, MAXM> wb{};
        array<double, MAXM> wc{};
        array<double, MAXM> wd{};
        array<double, MAXM> eps{};
        // 整数版 (整数演算高速化用, wa_int = round(wa * 1000))
        array<int, MAXM> wa_int{};
        array<int, MAXM> wb_int{};
        array<int, MAXM> wc_int{};
        array<int, MAXM> wd_int{};
        // 一手目の敵の行動 (p=1..M-1)
        i8 first_to[MAXM]{};
    };
    vector<SharedPlayoutParams> shared_playout_params;
    int shared_playout_params_size = 0;
    void reset_shared_playout_params() {
        shared_playout_params.clear();
        shared_playout_params_size = 0;
    }

    SharedPlayoutParams &
    get_shared_playout_params_by_playout_index(int playout_index_0based,
                                               const GameState &st) {
        int slot_idx = playout_index_0based % MAX_SHARED_PLAYOUT_PARAMS;
        while (shared_playout_params_size <= slot_idx) {
            rng.reset(playout_index_0based);
            SharedPlayoutParams params;
            for (int p = 1; p < input.M; p++) {
                estimator.sample_params(p, params.wa[p], params.wb[p],
                                        params.wc[p], params.wd[p],
                                        params.eps[p]);
                params.wa_int[p] = (int)lround(params.wa[p] * 1000);
                params.wb_int[p] = (int)lround(params.wb[p] * 1000);
                params.wc_int[p] = (int)lround(params.wc[p] * 1000);
                params.wd_int[p] = (int)lround(params.wd[p] * 1000);
                // 一手目の敵行動を現在の state で計算
                if (input.U == 1) {
                    params.first_to[p] =
                        simulate_ai_move_u1(p, st, params.wa_int[p],
                                            params.wc_int[p], params.eps[p]);
                } else {
                    params.first_to[p] = simulate_ai_move(
                        p, st, params.wa_int[p], params.wb_int[p],
                        params.wc_int[p], params.wd_int[p], params.eps[p]);
                }
            }
            shared_playout_params.push_back(params);
            shared_playout_params_size++;
        }
        return shared_playout_params[slot_idx];
    }

    // 自手のgreedy選択 (プレイアウト内で使用)
    // 安全な自陣マスをV最大(+切断点)に絞るフィルタをskip_maskで統合。
    // Pass1: own_cand & ~enemy_cand_mask を走査してmax_v算出
    // Pass2: own_cand を走査してskip_mask構築
    //        (no_reinforce / level-U安全 / V<max_v非切断点 をスキップ)
    // 評価ループ: cand_mask & ~skip_mask のみ走査 (分岐ゼロ, eval_self削減)
    int greedy_move_self(const GameState &st, int turn) {
        u128 cand_mask = st.get_cand_mask(0);

        const bool no_reinforce =
            (st.U != 1 && turn < CURRENT_NO_REINFORCE_TURNS);

        // Pass 1: 安全な空きマス or 自陣マス(level<U)の最大V値を求める
        // safe判定: no_enemy_prob が高い (敵が来にくい) セルを安全とみなす
        // 閾値: 0.5f (誰かが50%以上の確率で来るなら「脅威あり」)
        constexpr float SAFE_THRESHOLD = 0.5f;
        double max_eval = -1e18;
        int best_cell = -1;
        u128 m = cand_mask;
        int max_v_1 = -1;
        int max_v_cells[MAXCELLS];
        int max_v_cells_cnt = 0;
        int max_v_2 = -1;
        while (m) {
            int cell = ctz128(m);
            m &= m - 1;
            int lv = st.level[cell];
            int o = st.owner[cell];
            int v = input.V[cell];
            bool safe = (st.no_enemy_prob[cell] >= SAFE_THRESHOLD);
            if (v > max_v_2 && ((o == 0 && lv < st.U) || (o == -1 && safe))) {
                max_v_2 = v;
            }
            if (o == 0) {
                // この基準大丈夫か？意外と良いらしい
                if (lv == st.U && safe) {
                    // 安全な level-U 自陣: 常にスキップ
                    continue;
                } else if (lv < st.U) {
                    if (no_reinforce) {
                        // no-reinforce ターン: 全 own level<U をスキップ
                        continue;
                    } else if (safe) {
                        // 安全な level<U 自陣: max_v_1 候補
                        if (v > max_v_1) {
                            max_v_1 = v;
                            max_v_cells[0] = cell;
                            max_v_cells_cnt = 1;
                        } else if (v == max_v_1) {
                            max_v_cells[max_v_cells_cnt++] = cell;
                        }
                        continue;
                    }
                }
            } else if (o > 0 && o != st.top_enemy && st.level[cell] > 1) {
                // レベル4以上の非トップ敵陣マスを除外
                continue;
            }
            double e = st.eval_self(cell);
            e *= rng.randomRangeDouble(CURRENT_SELF_EVAL_NOISE_LO, 1.0);
            if (e > max_eval) {
                max_eval = e;
                best_cell = cell;
            }
        }
        if (max_v_1 >= max_v_2) {
            for (int i = 0; i < max_v_cells_cnt; i++) {
                double e = st.eval_self(max_v_cells[i]);
                e *= rng.randomRangeDouble(CURRENT_SELF_EVAL_NOISE_LO, 1.0);
                if (e > max_eval) {
                    max_eval = e;
                    best_cell = max_v_cells[i];
                }
            }
        }

        if (best_cell == -1)
            return st.pos[0];
        return best_cell;
    }

    // 候補手のフィルタリング (レベルU & 攻撃されない自領土を除外 +
    //   安全な自領土はV最大のみ残す、ただし切断点は例外)
    void filter_candidates(CandList &my_cands) {
        bool threatened[MAXCELLS] = {};
        for (int p = 1; p < state.M; p++) {
            CandList p_cands;
            state.get_candidates(p, p_cands);
            for (int k = 0; k < p_cands.n; k++)
                threatened[p_cands.ids[k]] = true;
        }

        // 安全な空きマス or
        // (安全かどうかによらず)自陣マス(level<U)のうち最大V値を求める
        int max_v_safe_own = -1;
        for (int k = 0; k < my_cands.n; k++) {
            int cell = my_cands.ids[k];
            if (input.V[cell] > max_v_safe_own &&
                ((state.owner[cell] == -1 && !threatened[cell]) ||
                 (state.owner[cell] == 0 && state.level[cell] < state.U))) {
                max_v_safe_own = input.V[cell];
            }
        }

        CandList filtered;
        filtered.clear();
        for (int k = 0; k < my_cands.n; k++) {
            int cell = my_cands.ids[k];
            int o = state.owner[cell];
            int lv = state.level[cell];
            if (o == 0 && !threatened[cell]) {
                // レベルMAXかつ安全な自陣マスを除外
                if (lv == state.U)
                    continue;
                // 安全な自陣マス(level<U)はV最大のもの以外は除外
                if (lv < state.U && input.V[cell] < max_v_safe_own)
                    continue;
            }
            // レベル4以上の非トップ敵陣マスを除外
            if (o > 0 && o != state.top_enemy && lv > 1)
                continue;
            filtered.push_id(cell);
        }
        if (filtered.n > 0)
            my_cands = filtered;
    }

    // 単一プレイアウトを実行し評価値を返す (base_ratioからの差分)
    // 呼び出し前に sim は一手目適用済みであること
    double run_playout(int t, double base_ratio,
                       const SharedPlayoutParams &params) {
        int T = input.T, M = input.M;

        i8 to[MAXM];
        int depth = min(T - t - 1, CURRENT_MAX_PLAYOUT_DEPTH);
        for (int s = 0; s < depth; s++) {
            // no_enemy_prob をプレイヤー0の候補セルで1に初期化
            memcpy(sim.no_enemy_prob, NO_ENEMY_PROB_INIT.data(),
                   sizeof(sim.no_enemy_prob));
            // 敵の手を決定しつつ no_enemy_prob を更新
            if (input.U == 1) {
                for (int p = 1; p < M; p++)
                    to[p] = simulate_ai_move_u1_and_update_prob(
                        p, sim, params.wa_int[p], params.wc_int[p],
                        params.eps[p]);
            } else {
                for (int p = 1; p < M; p++)
                    to[p] = simulate_ai_move_and_update_prob(
                        p, sim, params.wa_int[p], params.wb_int[p],
                        params.wc_int[p], params.wd_int[p], params.eps[p]);
            }
            to[0] = greedy_move_self(sim, t + 1 + s);
            if (s < depth - 1)
                sim.apply_moves_fast(to);
            else
                sim.apply_moves_fast_no_rebuild(to);
        }

        // score[] は apply_moves_fast 内で V*level
        // ベースにインクリメンタル更新 されているため、calc_scores
        // の再計算は不要
        int sa = sim.score[sim.top_enemy];
        double ratio = (sa > 0) ? (double)sim.score[0] / sa : base_ratio;
        return log2(ratio); // TODO: ratio?log(ratio)?sqrt(ratio)?ratio^2?
    }

    // ターンごとの時間配分の重みを事前計算
    void precompute_time_weights() {
        int T = input.T;
        for (int t = 0; t < T; t++) {
            double progress = (T > 1) ? (double)t / (T - 1) : 0.0;
            turn_weight[t] = 1.0 + (PLAYOUT_TIME_RATIO - 1.0) *
                                       pow(progress, PLAYOUT_TIME_EXP);
        }
        turn_weight_suffix[T] = 0.0;
        for (int t = T - 1; t >= 0; t--)
            turn_weight_suffix[t] = turn_weight_suffix[t + 1] + turn_weight[t];
    }

    // このターンの時間制限を計算
    double calc_turn_time_limit(int t) {
        double time_now = timer.elapsed();
        double remaining_time = TIME_LIMIT - time_now;
        return time_now +
               remaining_time * turn_weight[t] / turn_weight_suffix[t];
    }

    void output_move(int cell) {
        cout << state.id_x(cell) << " " << state.id_y(cell) << endl;
    }

  public:
    void solve() {
        input.read();
        init_params(input.M, input.U); // U×Mに応じたパラメータ初期化
        shared_playout_params.reserve(MAX_SHARED_PLAYOUT_PARAMS);
        state.init();
        estimator.init(input.M);
        sim.copy_topology_from(state);
        prev_state.copy_topology_from(state);

        int T = input.T, M = input.M;
        long long total_playout_count = 0;

        precompute_time_weights();

        for (int t = 0; t < T; t++) {
            double time_limit_this_turn = calc_turn_time_limit(t);

            // 時間余裕係数: (残り時間) / (当初予定でt以降に使えた時間)
            // 1.0未満 → 遅れ気味 → 打ち切りを早める
            // 1.0超   → 余裕あり → 打ち切りを遅らせる
            double planned_remaining =
                TIME_LIMIT * turn_weight_suffix[t] / turn_weight_suffix[0];
            double time_coeff =
                planned_remaining > 0.0
                    ? (TIME_LIMIT - timer.elapsed()) / planned_remaining
                    : 1.0;
            int early_exit_threshold =
                max(1, (int)((CURRENT_EARLY_EXIT_THRESHOLD_M2 -
                              (CURRENT_EARLY_EXIT_THRESHOLD_M2 -
                               CURRENT_EARLY_EXIT_THRESHOLD_M8) *
                                  (M - 2) / 6) *
                             time_coeff));

            // ベイズ更新
            if (t >= 1) {
                for (int p = 1; p < M; p++) {
                    estimator.update(p, prev_to[p], prev_state);
                }
            }

            // === M=2 特化: ターン20未満で敵到達可能領土が隣接不可の場合
            // ===
            if (M == 2 && t < 20) {
                // プレイヤー0の到達可能領土を計算
                u128 owned0 = state.owner_mask[0];
                u128 reachable0 = (u128)1 << state.pos[0];
                u128 prev0 = 0;
                while (reachable0 != prev0) {
                    prev0 = reachable0;
                    reachable0 |= state.expand(reachable0) & owned0;
                }
                // 到達可能領土の隣接マス集合（自領土外）
                u128 adj0 =
                    state.expand(reachable0) & ~reachable0 & state.board_mask;

                // プレイヤー1の到達可能領土を計算
                u128 owned1 = state.owner_mask[1];
                u128 reachable1 = (u128)1 << state.pos[1];
                u128 prev1 = 0;
                while (reachable1 != prev1) {
                    prev1 = reachable1;
                    reachable1 |= state.expand(reachable1) & owned1;
                }

                // プレイヤー1の到達可能領土が自分の隣接マス集合に存在しない場合
                if ((reachable1 & adj0) == 0) {
                    // BFSで到達可能領土からプレイヤー1の到達可能領土への最短パスを探す
                    // 各セルについて (距離, そこに至る最大累積スコア,
                    // 一手目のセル) を記録
                    int N = state.N, C = state.C;
                    int dist_bfs[MAXCELLS];
                    i64 best_score_bfs
                        [MAXCELLS]; // そのセルに到達するまでの最大累積V
                    int first_move[MAXCELLS]; // 一手目のセルID
                    memset(dist_bfs, -1, sizeof(dist_bfs));
                    memset(best_score_bfs, 0, sizeof(best_score_bfs));
                    memset(first_move, -1, sizeof(first_move));

                    // BFSキュー: (セルID)
                    // 到達可能領土を距離0として初期化
                    int q[MAXCELLS], qh = 0, qt = 0;
                    u128 r0 = reachable0;
                    while (r0) {
                        int b = ctz128(r0);
                        r0 &= r0 - 1;
                        dist_bfs[b] = 0;
                        best_score_bfs[b] = 0;
                        first_move[b] = -1;
                        q[qt++] = b;
                    }

                    // 他プレイヤーの駒位置を除外
                    u128 blocked = (u128)1 << state.pos[1];

                    int target_dist = -1; // 敵到達可能領土に到達する最短距離
                    int best_first = -1;
                    i64 best_path_score = -1;

                    while (qh < qt) {
                        int v = q[qh++];
                        int dv = dist_bfs[v];
                        // 既に最短距離の敵領土が見つかり、今のセルがそれより遠いなら終了
                        if (target_dist >= 0 && dv > target_dist)
                            break;
                        // このセルが敵到達可能領土なら記録
                        if (dv > 0 && ((reachable1 >> v) & 1)) {
                            if (target_dist < 0)
                                target_dist = dv;
                            if (best_score_bfs[v] > best_path_score) {
                                best_path_score = best_score_bfs[v];
                                best_first = first_move[v];
                            }
                        }
                        for (int k = 0; k < state.adj_deg[v]; k++) {
                            int nv = state.adj[v][k];
                            if ((blocked >> nv) & 1)
                                continue;
                            i64 new_score = best_score_bfs[v] + input.V[nv];
                            if (dist_bfs[nv] < 0) {
                                dist_bfs[nv] = dv + 1;
                                best_score_bfs[nv] = new_score;
                                if (dv == 0) {
                                    // 一手目: 隣接マスそのものが一手目
                                    first_move[nv] = nv;
                                } else {
                                    first_move[nv] = first_move[v];
                                }
                                q[qt++] = nv;
                            } else if (dist_bfs[nv] == dv + 1 &&
                                       new_score > best_score_bfs[nv]) {
                                // 同距離でスコアが良いパスを更新
                                best_score_bfs[nv] = new_score;
                                if (dv == 0) {
                                    first_move[nv] = nv;
                                } else {
                                    first_move[nv] = first_move[v];
                                }
                            }
                        }
                    }

                    if (best_first >= 0) {
                        output_move(best_first);
                        read_turn_result();
                        continue;
                    }
                }
            }
            // === M=2 特化ここまで ===

            // 候補手列挙 & フィルタリング
            CandList my_cands;
            state.get_candidates(0, my_cands);
            [[maybe_unused]] int before_n = my_cands.n;
            filter_candidates(my_cands);
            if (input.U != 1 && t < CURRENT_NO_REINFORCE_TURNS) {
                CandList filtered_no_reinforce;
                filtered_no_reinforce.clear();
                for (int k = 0; k < my_cands.n; k++) {
                    int cell = my_cands.ids[k];
                    if (state.owner[cell] == 0 && state.level[cell] < state.U)
                        continue;
                    filtered_no_reinforce.push_id(cell);
                }
                if (filtered_no_reinforce.n > 0)
                    my_cands = filtered_no_reinforce;
            }
            if constexpr (DEBUG) {
                cerr << "Turn " << t << ": filter " << before_n << " -> "
                     << my_cands.n << '\n';
            }

            // 候補が1つ以下なら即出力
            if (my_cands.n <= 1) {
                int out_cell =
                    (my_cands.n == 1) ? my_cands.ids[0] : state.pos[0];
                if constexpr (SIM_MODE) {
                    simulate_turn_result(out_cell);
                } else {
                    output_move(out_cell);
                    read_turn_result();
                }
                continue;
            }

            // 現在のスコア比をベースラインとして計算
            // state.score は read_turn_result 内で calc_scores 済み
            int base_sa = 0;
            for (int p = 1; p < input.M; p++)
                base_sa = max(base_sa, state.score[p]);
            double base_ratio =
                (base_sa > 0) ? (double)state.score[0] / base_sa : 1.0;

            // プレイアウトで各手を評価 (差分ベース)
            int n_cands = my_cands.n;
            double total_score[MAXCELLS] = {};
            double total_score_sq[MAXCELLS] = {};
            int count[MAXCELLS] = {};
            int playout_count = 0;
            reset_shared_playout_params();

            int consecutive_same = 0;
            int last_ci = -1;

            while (timer.elapsed() < time_limit_this_turn) {
                int ci = select_root_ucb(total_score, total_score_sq, count,
                                         n_cands, playout_count + 1);
                if (ci == last_ci) {
                    consecutive_same++;
                } else {
                    consecutive_same = 1;
                    last_ci = ci;
                }
                int playout_idx_for_ci = count[ci];
                SharedPlayoutParams &params =
                    get_shared_playout_params_by_playout_index(
                        playout_idx_for_ci, state);
                rng.reset(playout_idx_for_ci); //
                //     同一手は同一乱数列で評価（ノイズの影響を平均化）
                // run_playout 前に sim へコピーし、一手目を適用
                sim.copy_dynamic_from(state);
                params.first_to[0] = my_cands.ids[ci];
                sim.apply_moves_fast(params.first_to);
                double eval = run_playout(t, base_ratio, params);
                total_score[ci] += eval;
                total_score_sq[ci] += eval * eval;
                count[ci]++;
                playout_count++;
                // 同一手がearly_exit_threshold回連続で選ばれたら早期終了
                if (n_cands >= 2 && consecutive_same >= early_exit_threshold) {
                    if constexpr (DEBUG) {
                        cerr << "[EARLY_EXIT] t=" << t << " ci=" << last_ci
                             << " cell=" << (int)my_cands.ids[last_ci]
                             << " playouts=" << playout_count << '\n';
                    }
                    break;
                }
                if constexpr (DEBUG) {
                    if (playout_count % 10 == 0) {
                        // 各候補の平均スコアを計算してランク付け
                        double means[MAXCELLS] = {};
                        for (int i = 0; i < n_cands; i++) {
                            means[i] = (count[i] > 0)
                                           ? total_score[i] / count[i]
                                           : -1e18;
                        }
                        int ranks[MAXCELLS] = {};
                        for (int i = 0; i < n_cands; i++) {
                            int r = 1;
                            for (int j = 0; j < n_cands; j++) {
                                if (means[j] > means[i])
                                    r++;
                            }
                            ranks[i] = r;
                        }
                        cerr << "[PLAYOUT_LOG] t=" << t
                             << " p=" << playout_count << " n=" << n_cands;
                        for (int i = 0; i < n_cands; i++) {
                            int cell = my_cands.ids[i];
                            double mean = means[i];
                            double var = (count[i] > 1)
                                             ? (total_score_sq[i] / count[i] -
                                                mean * mean)
                                             : 0.0;
                            cerr << " " << i << "," << state.id_x(cell) << ","
                                 << state.id_y(cell) << "," << count[i] << ","
                                 << mean << "," << var << "," << ranks[i];
                        }
                        cerr << '\n';
                    }
                }
            }
            total_playout_count += playout_count;

            // 最良の手を選択
            int best_ci = 0;
            double best_avg = -1e18;
            for (int i = 0; i < n_cands; i++) {
                if (count[i] > 0) {
                    double avg = total_score[i] / count[i];
                    if (avg > best_avg) {
                        best_avg = avg;
                        best_ci = i;
                    }
                }
            }

            if constexpr (DEBUG) {
                cerr << "Turn " << t << ": cands=" << n_cands
                     << " playouts=" << playout_count
                     << " best_avg=" << best_avg << '\n';
            }

            if constexpr (SIM_MODE) {
                simulate_turn_result(my_cands.ids[best_ci]);
            } else {
                output_move(my_cands.ids[best_ci]);
                read_turn_result();
            }
        }
        if constexpr (DEBUG) {
            // デバッグ出力
            for (int p = 1; p < input.M; p++) {
                double wa, wb, wc, wd, eps;
                estimator.map_params(p, wa, wb, wc, wd, eps);
                cerr << "Player " << p << ": wa=" << wa << " wb=" << wb
                     << " wc=" << wc << " wd=" << wd << " eps=" << eps << '\n';
            }
        }
        cerr << "Total playouts: " << total_playout_count << '\n';
    }

    // シミュレーションモード用: 敵の手をサンプリングして apply_moves_fast
    // で状態更新
    void simulate_turn_result(int my_cell) {
        int M = input.M;
        prev_state.copy_dynamic_from(state);

        i8 to[MAXM];
        to[0] = (i8)my_cell;

        // 敵の手をサンプリングして決定
        if (input.U == 1) {
            for (int p = 1; p < M; p++) {
                double wa, wb, wc, wd, eps;
                estimator.sample_params(p, wa, wb, wc, wd, eps);
                to[p] = (i8)simulate_ai_move_u1(p, state, wa, wc, eps);
            }
        } else {
            for (int p = 1; p < M; p++) {
                double wa, wb, wc, wd, eps;
                estimator.sample_params(p, wa, wb, wc, wd, eps);
                to[p] = (i8)simulate_ai_move(p, state, wa, wb, wc, wd, eps);
            }
        }

        // prev_to を記録（次ターンのベイズ更新用）
        for (int p = 0; p < M; p++) {
            prev_to[p] = to[p];
        }

        // apply_moves_fast で状態更新
        // (score, owner_mask, reachable_mask, players_mask, top_enemy
        // も更新される)
        state.apply_moves_fast(to);
    }

    // 変更3: topology不変なのでcopy_dynamic_fromで十分
    void read_turn_result() {
        int M = input.M, C = input.N * input.N;
        prev_state.copy_dynamic_from(state);

        int tx, ty;
        rep(p, M) {
            cin >> tx >> ty;
            prev_to[p] = state.to_id(tx, ty);
        }
        rep(p, M) {
            int x, y;
            cin >> x >> y;
            state.pos[p] = state.to_id(x, y);
        }
        for (int cell = 0; cell < C; cell++) {
            int v;
            cin >> v;
            state.owner[cell] = v;
        }
        for (int cell = 0; cell < C; cell++) {
            int v;
            cin >> v;
            state.level[cell] = v;
        }
        // owner_mask を再構築
        memset(state.owner_mask, 0, sizeof(state.owner_mask));
        for (int cell = 0; cell < C; cell++) {
            int o = state.owner[cell];
            if (o >= 0)
                state.owner_mask[o] |= (u128)1 << cell;
        }
        // score, enemy順位を再計算
        state.calc_scores(state.score);
        for (int p = 1; p < M; p++) {
            if (state.score[p] > state.score[state.top_enemy])
                state.top_enemy = p;
        }
        // reachable_mask / players_mask を再計算
        state.players_mask = 0;
        for (int p = 0; p < M; p++) {
            state.rebuild_reachable_mask(p);
            state.players_mask |= (u128)1 << state.pos[p];
        }
    }
};

int main() {
    Solver solver;
    solver.solve();
    cout.flush();
    cerr.flush();
    _Exit(0);
}
