// last local submission: 1733730518
// このコードを見に来た方へ：
// 今回はコンテスト中盤以降苦し紛れに実装したものが多く、失敗作の残骸も多いため、自分含め誰も読めないと思います
package main

import (
	"bufio"
	"fmt"
	"math"
	"math/bits"
	"math/rand"
	"os"
	"sort"
	"strconv"
	"strings"
	"sync"
	"time"
)

const (
	INF = 10000000000000
	// Main
	TIME_RATE         = 1.0
	TIME_LIMIT        = 2850 * 1000 * TIME_RATE
	TIME_LIMIT_STRICT = 2950 * 1000 * TIME_RATE

	SAMPLE_NUM    = 5
	ADAPTIVE      = true
	TURNING_POINT = 1

	MINIMIZE = true

	LOCAL_TEST = false // テストファイルを利用するか

	SUCCESSIVE         = true // 複数ケースを試すか
	USE_FILE           = true
	REPEAT             = false // 同一ケースを回すか
	TEST_NUM           = 300
	PARALLEL_NUM       = 6 // 6
	PREFIX             = 0 // nケース目から始めたい場合
	OUT_FOLDER         = "ignore\\out.txt"
	FILE               = "ignore\\in"    // テストケースの保存場所
	FILE_NUMBER        = "2844"          // 単一ケースを試す場合の番号
	RECORD             = true            // nケース結果を保存するか
	RECORD_FOLDER      = "ignore\\tries" // nケース結果保存場所
	RECORD_CODE        = true
	CODE_FILE          = "ignore\\code" // テストケースの保存場所
	USE_RELATIVE_SCORE = false
	BEST_FILE          = "ignore\\tries\\bests.txt"
	DEBUG              = true
	PRINT_ACTION       = true // 標準出力へ行動を出力するか

	VERSION_MAJOR = 0              // 基本戦術、方針レベルの違い
	VERSION_MINOR = 0              // 改善等
	VERSION_PATCH = 0              // バグ修正,パラメータ変更など
	VERSION_NAME  = "estimate3.0s" // バージョン名
	VERSION_VALID = true           // 提出に利用できる正当なものか

)

var HASH_BORDERLINE = [100]uint64{8717895732742165505, 2259404117704393152, 6050128673802995827, 501233450539197794, 3390393562759376202, 2669985732393126063, 1774932891286980153, 6044372234677422456, 8274930044578894929, 1543572285742637646, 2661732831099943416, 8325060299420976708, 7837839688282259259, 2518412263346885298, 5617773211005988520, 2339563716805116249, 7144924247938981575, 161231572858529631, 7259475919510918339, 7373105480197164748, 3287288577352441706, 3930927879439176946, 4706154865122290029, 2217592893536642650, 1929546706668609706, 6392442863481646880, 3706853784096366226, 2627038740284806767, 6303220950515014660, 4035568504096476779, 959367522974354090, 2914295034816259174, 1395437218309923052, 6745438398739480977, 2897681629866238117, 3398579248012586914, 5974317550424871033, 3317123977833389635, 5001958211167890979, 167566062957544642, 4778690082005258714, 1059542851699319360, 6972732843819909978, 5558237345453186302, 7845762441295307478, 771642788862502430, 8514850266767180993, 8683452355129068124, 4345851588384648695, 7699391924090763411, 388440063886460141, 7561811714888168464, 3959279844101328186, 8902041070398994519, 303089054982227392, 7338728586234333996, 5392504858645185670, 7847956203786849690, 406703151708498928, 4756106358532488297, 5837486892148644279, 4736217237333769909, 2264299874001785192, 1061380815263676471, 7242748068272024738, 7719717197379695442, 4112921325496946042, 2671030200101705776, 3508963237347473586, 8565714761387219319, 4564823113789767141, 1198006251912892506, 7014402135919778893, 3983722386484812742, 2118716725206170867, 2587000937929698613, 8489437897698681073, 1938800996802160635, 8097022081922209513, 7481608503761597087, 6056649900269286653, 8056746523676181822, 4308690457412179793, 7663837986485606015, 7132033595893905170, 8086159467323165929, 35604086129376003, 8559453321117178323, 2006924026344156168, 8218430188258725598, 4255970180603226314, 2682844416202521633, 4304520335772049496, 3462733497206508461, 5902760509050140210, 9021104375654741729, 3604381780091280195, 2066195468801476818, 3331863358128628835, 759605945513541974}
var HASH_BORDERLINE_MINUS = [100]uint64{2408550365227740434, 5521202747878656476, 5596430475431407243, 6651829488660799814, 8482125374365136680, 7862762095958642309, 4391202566038595699, 6215632031706852400, 241482278320610612, 7442289190031176026, 5364953311572054685, 7945398411639602224, 1925401661646756611, 150340687756601720, 4989862523986425397, 2803285039048912676, 950400323440343118, 6430969915190400444, 1937101031588528881, 6604365855503062775, 1836598054518427835, 7540276489530073149, 7638413271565042464, 3242614188194728891, 6521671820626549617, 434400178965901716, 1891001667378689416, 1627381309359808899, 8204648627352676445, 4234137922270959652, 8497925768463229012, 5311927246208705713, 3967212276624460248, 1681876124477381252, 1115785012616387305, 2629911606854649819, 8392001091488039958, 6882849783541559690, 6018839464190747916, 2037591971392316788, 6394356307858046544, 9096429817347931519, 5026609382502824278, 2718877847597668777, 2333048574390956331, 9205243623417456715, 190417550815006435, 7478610059307147871, 4238649515632009295, 544981646038740619, 4814861198247358488, 4975249678507640420, 8953538234431013647, 4540487686588600123, 5310832663795041070, 1363585710475529225, 8279128640960530079, 1011676084465510524, 8764227983217623240, 4745905187492708501, 7941830299651147569, 157519078836327761, 2867593906384393455, 7506934391669544280, 952897656927189675, 8835845053628448511, 3874550043338258151, 3755969145755718156, 3661602461251866513, 1899012902909494361, 1062424578646559011, 3321710981400855005, 4230816687517220040, 6165970817952435057, 6017140934898985776, 7953274849279451463, 2223751782546645906, 8958290475970215309, 6870143829354119039, 4400124260933614083, 8532234679993278697, 2151743514245058837, 3383203076453688632, 330151684706709734, 4984797317908301849, 7686248226181626741, 4970157864765978097, 3909624772458770597, 1377327594979300801, 2654595716993425044, 4283016341703943597, 8997481548049309375, 8271791276134687140, 2606774689767964810, 8003520668589102978, 5418224491453948590, 7899896093082851758, 4680282486764958852, 6464511094049078446, 5857858779299113932}

var ds = [501]float64{0.5000000000, 0.4960106437, 0.4920216863, 0.4880335266, 0.4840465631, 0.4800611942, 0.4760778173, 0.4720968298, 0.4681186280, 0.4641436074, 0.4601721627, 0.4562046875, 0.4522415740, 0.4482832133, 0.4443299952, 0.4403823076, 0.4364405371, 0.4325050683, 0.4285762841, 0.4246545653, 0.4207402906, 0.4168338365, 0.4129355774, 0.4090458849, 0.4051651283, 0.4012936743, 0.3974318868, 0.3935801268, 0.3897387524, 0.3859081188, 0.3820885778, 0.3782804782, 0.3744841653, 0.3706999811, 0.3669282640, 0.3631693488, 0.3594235668, 0.3556912452, 0.3519727076, 0.3482682735, 0.3445782584, 0.3409029738, 0.3372427268, 0.3335978206, 0.3299685537, 0.3263552203, 0.3227581103, 0.3191775088, 0.3156136965, 0.3120669494, 0.3085375387, 0.3050257309, 0.3015317875, 0.2980559654, 0.2945985162, 0.2911596868, 0.2877397188, 0.2843388490, 0.2809573089, 0.2775953248, 0.2742531178, 0.2709309038, 0.2676288935, 0.2643472921, 0.2610862997, 0.2578461108, 0.2546269147, 0.2514288951, 0.2482522305, 0.2450970937, 0.2419636522, 0.2388520681, 0.2357624978, 0.2326950923, 0.2296499972, 0.2266273524, 0.2236272924, 0.2206499463, 0.2176954376, 0.2147638842, 0.2118553986, 0.2089700879, 0.2061080536, 0.2032693918, 0.2004541933, 0.1976625431, 0.1948945213, 0.1921502021, 0.1894296548, 0.1867329430, 0.1840601253, 0.1814112549, 0.1787863796, 0.1761855422, 0.1736087803, 0.1710561263, 0.1685276075, 0.1660232461, 0.1635430593, 0.1610870595, 0.1586552539, 0.1562476450, 0.1538642304, 0.1515050028, 0.1491699503, 0.1468590564, 0.1445722997, 0.1423096544, 0.1400710901, 0.1378565720, 0.1356660609, 0.1334995132, 0.1313568810, 0.1292381122, 0.1271431506, 0.1250719356, 0.1230244031, 0.1210004844, 0.1190001075, 0.1170231960, 0.1150696702, 0.1131394464, 0.1112324374, 0.1093485524, 0.1074876971, 0.1056497737, 0.1038346811, 0.1020423151, 0.1002725680, 0.0985253290, 0.0968004846, 0.0950979178, 0.0934175090, 0.0917591357, 0.0901226725, 0.0885079914, 0.0869149619, 0.0853434508, 0.0837933224, 0.0822644387, 0.0807566592, 0.0792698415, 0.0778038405, 0.0763585095, 0.0749336995, 0.0735292596, 0.0721450370, 0.0707808770, 0.0694366233, 0.0681121180, 0.0668072013, 0.0655217121, 0.0642554878, 0.0630083645, 0.0617801767, 0.0605707580, 0.0593799406, 0.0582075556, 0.0570534332, 0.0559174025, 0.0547992917, 0.0536989281, 0.0526161385, 0.0515507485, 0.0505025835, 0.0494714680, 0.0484572263, 0.0474596818, 0.0464786579, 0.0455139773, 0.0445654628, 0.0436329365, 0.0427162208, 0.0418151376, 0.0409295090, 0.0400591569, 0.0392039033, 0.0383635704, 0.0375379803, 0.0367269557, 0.0359303191, 0.0351478936, 0.0343795024, 0.0336249694, 0.0328841187, 0.0321567748, 0.0314427630, 0.0307419089, 0.0300540390, 0.0293789800, 0.0287165598, 0.0280666067, 0.0274289497, 0.0268034189, 0.0261898449, 0.0255880595, 0.0249978951, 0.0244191853, 0.0238517643, 0.0232954678, 0.0227501319, 0.0222155944, 0.0216916938, 0.0211782696, 0.0206751629, 0.0201822154, 0.0196992704, 0.0192261722, 0.0187627664, 0.0183088999, 0.0178644206, 0.0174291779, 0.0170030226, 0.0165858067, 0.0161773834, 0.0157776074, 0.0153863348, 0.0150034230, 0.0146287308, 0.0142621184, 0.0139034475, 0.0135525811, 0.0132093838, 0.0128737214, 0.0125454614, 0.0122244727, 0.0119106254, 0.0116037915, 0.0113038442, 0.0110106583, 0.0107241100, 0.0104440771, 0.0101704387, 0.0099030756, 0.0096418699, 0.0093867055, 0.0091374675, 0.0088940426, 0.0086563190, 0.0084241864, 0.0081975359, 0.0079762603, 0.0077602536, 0.0075494114, 0.0073436310, 0.0071428107, 0.0069468508, 0.0067556526, 0.0065691191, 0.0063871548, 0.0062096653, 0.0060365581, 0.0058677417, 0.0057031263, 0.0055426234, 0.0053861460, 0.0052336082, 0.0050849257, 0.0049400158, 0.0047987966, 0.0046611880, 0.0045271111, 0.0043964883, 0.0042692434, 0.0041453014, 0.0040245885, 0.0039070326, 0.0037925623, 0.0036811080, 0.0035726010, 0.0034669738, 0.0033641604, 0.0032640958, 0.0031667163, 0.0030719592, 0.0029797632, 0.0028900681, 0.0028028146, 0.0027179449, 0.0026354021, 0.0025551303, 0.0024770750, 0.0024011825, 0.0023274002, 0.0022556767, 0.0021859615, 0.0021182050, 0.0020523590, 0.0019883759, 0.0019262091, 0.0018658133, 0.0018071438, 0.0017501569, 0.0016948100, 0.0016410612, 0.0015888696, 0.0015381952, 0.0014889987, 0.0014412419, 0.0013948872, 0.0013498980, 0.0013062384, 0.0012638734, 0.0012227687, 0.0011828907, 0.0011442068, 0.0011066850, 0.0010702939, 0.0010350030, 0.0010007825, 0.0009676032, 0.0009354367, 0.0009042552, 0.0008740315, 0.0008447392, 0.0008163523, 0.0007888457, 0.0007621947, 0.0007363753, 0.0007113640, 0.0006871379, 0.0006636749, 0.0006409530, 0.0006189511, 0.0005976485, 0.0005770250, 0.0005570611, 0.0005377374, 0.0005190354, 0.0005009369, 0.0004834241, 0.0004664799, 0.0004500872, 0.0004342299, 0.0004188919, 0.0004040578, 0.0003897124, 0.0003758409, 0.0003624291, 0.0003494631, 0.0003369293, 0.0003248144, 0.0003131057, 0.0003017906, 0.0002908571, 0.0002802933, 0.0002700877, 0.0002602292, 0.0002507069, 0.0002415103, 0.0002326291, 0.0002240533, 0.0002157734, 0.0002077798, 0.0002000635, 0.0001926156, 0.0001854274, 0.0001784906, 0.0001717971, 0.0001653390, 0.0001591086, 0.0001530985, 0.0001473015, 0.0001417106, 0.0001363190, 0.0001311202, 0.0001261076, 0.0001212752, 0.0001166170, 0.0001121270, 0.0001077997, 0.0001036296, 0.0000996114, 0.0000957399, 0.0000920101, 0.0000884173, 0.0000849567, 0.0000816238, 0.0000784142, 0.0000753236, 0.0000723480, 0.0000694834, 0.0000667258, 0.0000640716, 0.0000615172, 0.0000590589, 0.0000566935, 0.0000544177, 0.0000522282, 0.0000501221, 0.0000480963, 0.0000461481, 0.0000442745, 0.0000424729, 0.0000407408, 0.0000390756, 0.0000374749, 0.0000359363, 0.0000344576, 0.0000330366, 0.0000316712, 0.0000303594, 0.0000290991, 0.0000278884, 0.0000267256, 0.0000256088, 0.0000245364, 0.0000235066, 0.0000225179, 0.0000215687, 0.0000206575, 0.0000197830, 0.0000189436, 0.0000181382, 0.0000173653, 0.0000166238, 0.0000159124, 0.0000152300, 0.0000145755, 0.0000139477, 0.0000133457, 0.0000127685, 0.0000122151, 0.0000116846, 0.0000111760, 0.0000106885, 0.0000102213, 0.0000097736, 0.0000093447, 0.0000089337, 0.0000085399, 0.0000081627, 0.0000078015, 0.0000074555, 0.0000071241, 0.0000068069, 0.0000065031, 0.0000062123, 0.0000059340, 0.0000056675, 0.0000054125, 0.0000051685, 0.0000049350, 0.0000047117, 0.0000044979, 0.0000042935, 0.0000040980, 0.0000039110, 0.0000037322, 0.0000035612, 0.0000033977, 0.0000032414, 0.0000030920, 0.0000029492, 0.0000028127, 0.0000026823, 0.0000025577, 0.0000024386, 0.0000023249, 0.0000022162, 0.0000021125, 0.0000020133, 0.0000019187, 0.0000018283, 0.0000017420, 0.0000016597, 0.0000015810, 0.0000015060, 0.0000014344, 0.0000013660, 0.0000013008, 0.0000012386, 0.0000011792, 0.0000011226, 0.0000010686, 0.0000010171, 0.0000009680, 0.0000009211, 0.0000008765, 0.0000008339, 0.0000007933, 0.0000007547, 0.0000007178, 0.0000006827, 0.0000006492, 0.0000006173, 0.0000005869, 0.0000005580, 0.0000005304, 0.0000005042, 0.0000004792, 0.0000004554, 0.0000004327, 0.0000004111, 0.0000003906, 0.0000003711, 0.0000003525, 0.0000003348, 0.0000003179, 0.0000003019, 0.0000002867}
var logds = [501]float64{0.000000, -0.003479, -0.006986, -0.010520, -0.014083, -0.017673, -0.021292, -0.024939, -0.028614, -0.032318, -0.036050, -0.039810, -0.043600, -0.047418, -0.051264, -0.055140, -0.059045, -0.062979, -0.066942, -0.070934, -0.074956, -0.079007, -0.083088, -0.087198, -0.091338, -0.095508, -0.099707, -0.103937, -0.108196, -0.112486, -0.116806, -0.121156, -0.125537, -0.129947, -0.134389, -0.138861, -0.143363, -0.147897, -0.152461, -0.157056, -0.161682, -0.166339, -0.171027, -0.175747, -0.180497, -0.185279, -0.190093, -0.194938, -0.199814, -0.204722, -0.209662, -0.214634, -0.219637, -0.224672, -0.229739, -0.234839, -0.239970, -0.245134, -0.250330, -0.255558, -0.260818, -0.266111, -0.271437, -0.276795, -0.282186, -0.287609, -0.293066, -0.298555, -0.304077, -0.309632, -0.315220, -0.320841, -0.326495, -0.332183, -0.337904, -0.343658, -0.349445, -0.355266, -0.361121, -0.367009, -0.372930, -0.378886, -0.384875, -0.390898, -0.396955, -0.403046, -0.409170, -0.415329, -0.421522, -0.427749, -0.434010, -0.440306, -0.446636, -0.453000, -0.459398, -0.465831, -0.472299, -0.478801, -0.485338, -0.491909, -0.498516, -0.505157, -0.511832, -0.518543, -0.525289, -0.532069, -0.538885, -0.545736, -0.552621, -0.559543, -0.566499, -0.573490, -0.580517, -0.587579, -0.594677, -0.601810, -0.608979, -0.616183, -0.623423, -0.630698, -0.638009, -0.645356, -0.652739, -0.660157, -0.667611, -0.675101, -0.682628, -0.690190, -0.697788, -0.705422, -0.713092, -0.720799, -0.728542, -0.736321, -0.744136, -0.751988, -0.759875, -0.767800, -0.775761, -0.783758, -0.791792, -0.799862, -0.807969, -0.816113, -0.824293, -0.832510, -0.840764, -0.849054, -0.857381, -0.865746, -0.874147, -0.882585, -0.891060, -0.899572, -0.908121, -0.916707, -0.925330, -0.933991, -0.942688, -0.951423, -0.960195, -0.969004, -0.977851, -0.986735, -0.995656, -1.004615, -1.013611, -1.022645, -1.031716, -1.040825, -1.049972, -1.059156, -1.068377, -1.077636, -1.086933, -1.096268, -1.105641, -1.115051, -1.124499, -1.133985, -1.143509, -1.153071, -1.162670, -1.172308, -1.181984, -1.191698, -1.201449, -1.211239, -1.221067, -1.230933, -1.240838, -1.250780, -1.260761, -1.270780, -1.280837, -1.290933, -1.301067, -1.311239, -1.321449, -1.331699, -1.341986, -1.352312, -1.362677, -1.373080, -1.383521, -1.394001, -1.404520, -1.415077, -1.425673, -1.436308, -1.446981, -1.457693, -1.468444, -1.479233, -1.490062, -1.500929, -1.511835, -1.522780, -1.533763, -1.544786, -1.555848, -1.566948, -1.578087, -1.589266, -1.600483, -1.611740, -1.623035, -1.634370, -1.645744, -1.657157, -1.668609, -1.680100, -1.691630, -1.703200, -1.714809, -1.726457, -1.738144, -1.749871, -1.761637, -1.773442, -1.785287, -1.797171, -1.809094, -1.821057, -1.833059, -1.845101, -1.857182, -1.869303, -1.881463, -1.893663, -1.905902, -1.918181, -1.930499, -1.942857, -1.955255, -1.967692, -1.980169, -1.992685, -2.005242, -2.017838, -2.030473, -2.043149, -2.055864, -2.068619, -2.081414, -2.094249, -2.107123, -2.120037, -2.132991, -2.145985, -2.159019, -2.172093, -2.185207, -2.198361, -2.211555, -2.224788, -2.238062, -2.251376, -2.264729, -2.278123, -2.291557, -2.305031, -2.318545, -2.332099, -2.345693, -2.359327, -2.373002, -2.386717, -2.400472, -2.414267, -2.428102, -2.441977, -2.455893, -2.469849, -2.483845, -2.497882, -2.511959, -2.526076, -2.540233, -2.554431, -2.568669, -2.582948, -2.597266, -2.611626, -2.626025, -2.640465, -2.654946, -2.669467, -2.684028, -2.698630, -2.713273, -2.727956, -2.742679, -2.757443, -2.772247, -2.787092, -2.801978, -2.816904, -2.831871, -2.846878, -2.861926, -2.877015, -2.892144, -2.907314, -2.922524, -2.937775, -2.953067, -2.968400, -2.983773, -2.999187, -3.014642, -3.030137, -3.045673, -3.061250, -3.076868, -3.092527, -3.108226, -3.123966, -3.139747, -3.155569, -3.171431, -3.187335, -3.203279, -3.219264, -3.235290, -3.251357, -3.267465, -3.283614, -3.299804, -3.316034, -3.332306, -3.348619, -3.364972, -3.381367, -3.397802, -3.414279, -3.430796, -3.447355, -3.463954, -3.480595, -3.497276, -3.513999, -3.530763, -3.547568, -3.564414, -3.581300, -3.598229, -3.615198, -3.632208, -3.649260, -3.666352, -3.683486, -3.700661, -3.717877, -3.735135, -3.752433, -3.769772, -3.787153, -3.804575, -3.822039, -3.839543, -3.857089, -3.874676, -3.892304, -3.909973, -3.927685, -3.945437, -3.963230, -3.981065, -3.998941, -4.016858, -4.034816, -4.052816, -4.070858, -4.088940, -4.107064, -4.125230, -4.143437, -4.161685, -4.179975, -4.198305, -4.216677, -4.235090, -4.253546, -4.272043, -4.290581, -4.309159, -4.327780, -4.346442, -4.365146, -4.383892, -4.402678, -4.421507, -4.440376, -4.459288, -4.478240, -4.497234, -4.516270, -4.535347, -4.554467, -4.573629, -4.592830, -4.612073, -4.631356, -4.650684, -4.670053, -4.689464, -4.708915, -4.728405, -4.747939, -4.767517, -4.787136, -4.806792, -4.826493, -4.846240, -4.866021, -4.885850, -4.905718, -4.925622, -4.945578, -4.965572, -4.985605, -5.005683, -5.025792, -5.045960, -5.066159, -5.086398, -5.106682, -5.127005, -5.147374, -5.167785, -5.188237, -5.208731, -5.229266, -5.249847, -5.270463, -5.291120, -5.311829, -5.332566, -5.353361, -5.374173, -5.395062, -5.415963, -5.436923, -5.457922, -5.478940, -5.500038, -5.521145, -5.542300, -5.563519, -5.584759, -5.606039, -5.627383, -5.648745, -5.670155, -5.691606, -5.713095, -5.734663, -5.756218, -5.777856, -5.799533, -5.821196, -5.842967, -5.864740, -5.886591, -5.908474, -5.930406, -5.952336, -5.974366, -5.996367, -6.018453, -6.040577, -6.062783, -6.085023, -6.107238, -6.129479, -6.151811, -6.174185, -6.196679, -6.219107, -6.241542}

type RandGenerater struct {
	x int
}

func (rg *RandGenerater) Next() int {
	// 32bit乱数を生成
	return 0
}

func (rg *RandGenerater) IntN(n int) int {
	a := (rg.Next() * n) >> 32
	return a
}

func fastPow(a float64, e int) float64 {
	if e == 0 {
		return 1
	}
	k := 64 - bits.LeadingZeros(uint(e))
	ret := 1.0
	a1 := a
	for i := 0; i < k; i++ {
		if e&(1<<i) != 0 {
			ret *= a1
		}
		a1 *= a1
	}
	return ret
}

func maxInt(a int, b int) int {
	if a > b {
		return a
	}
	return b
}

func minInt(a int, b int) int {
	if a < b {
		return a
	}
	return b
}

func absInt(a int) int {
	if a < 0 {
		return -a
	}
	return a
}

func popcount(x int) int {
	x = x - ((x >> 1) & 0x5555555555555555)

	x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333)

	x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f
	x = x + (x >> 8)
	x = x + (x >> 16)
	x = x + (x >> 32)
	return x & 0x0000007f
}

func getScanner(file string) *bufio.Scanner {
	var scanner *bufio.Scanner
	if LOCAL_TEST && USE_FILE {
		fp, err := os.Open(file)
		if err != nil {
			panic("Error: Can not open file ")
		}
		if !REPEAT {
			fmt.Fprintf(os.Stderr, "---CASE %s---\n", file)
		}
		scanner = bufio.NewScanner(fp)
	} else {
		scanner = bufio.NewScanner(os.Stdin)
	}
	scanner.Buffer(make([]byte, 1000000), 1000000)
	return scanner
}

func getInputs(gConst *GameConst) {
	// 例
	gConst.Scanner.Scan()
	splitedInput := strings.Split(gConst.Scanner.Text(), " ")
	N, _ := strconv.Atoi(splitedInput[0])
	T, _ := strconv.Atoi(splitedInput[1])
	sigma, _ := strconv.Atoi(splitedInput[2])
	gConst.NumBox = N
	gConst.SqrtNumBox = int(math.Sqrt(float64(N)))
	gConst.NumQuery = T
	gConst.Sigma = sigma
	gConst.EstimatedSigma = int(float64(sigma) * math.Pow(0.4, float64((T-12))/float64(N)/2))
	// gConst.EstimatedSigma /= 2

	// 推定サイズを取得
	for loop := 0; loop < gConst.NumBox; loop++ {
		gConst.Scanner.Scan()
		splitedInput := strings.Split(gConst.Scanner.Text(), " ")
		w, _ := strconv.Atoi(splitedInput[0])
		h, _ := strconv.Atoi(splitedInput[1])
		// if w < 10000+sigma/2 {
		// 	w = 10000 + sigma/2
		// }
		// if w > 100000-sigma/2 {
		// 	w = 100000 - sigma/2
		// }
		// if h < 10000+sigma/2 {
		// 	h = 10000 + sigma/2
		// }
		// if h > 100000-sigma/2 {
		// 	h = 100000 - sigma/2
		// }
		gConst.InitialEstimatedSizes = append(gConst.InitialEstimatedSizes, [2]int{w, h})
	}

	gConst.bsPool = sync.Pool{
		New: func() interface{} {
			return NewBeamSearchState(gConst.NumBox)
		}}

	if LOCAL_TEST {
		// 正解のサイズを取得
		for loop := 0; loop < gConst.NumBox; loop++ {
			gConst.Scanner.Scan()
			splitedInput := strings.Split(gConst.Scanner.Text(), " ")
			w, _ := strconv.Atoi(splitedInput[0])
			h, _ := strconv.Atoi(splitedInput[1])
			gConst.TrueData.sizes = append(gConst.TrueData.sizes, [2]int{w, h})
		}
		// クエリに対する誤差を取得
		for loop := 0; loop < gConst.NumQuery; loop++ {
			gConst.Scanner.Scan()
			splitedInput := strings.Split(gConst.Scanner.Text(), " ")
			wError, _ := strconv.Atoi(splitedInput[0])
			hError, _ := strconv.Atoi(splitedInput[1])
			// wError := 0
			// hError := 0
			gConst.TrueData.errors = append(gConst.TrueData.errors, [2]int{wError, hError})
		}

	}
}

// 現在の状態を記録するもの
type GameState struct {
	placed    []bool   // 設置したか
	positions [][2]int // 各箱の左上座標　xmin,ymin
	rotated   []int    // 回転しているか

	progress int // 最後に置かれたboxid
}

func NewGameState(gConst *GameConst) *GameState {
	g := GameState{}
	g.placed = make([]bool, gConst.NumBox)
	g.positions = make([][2]int, gConst.NumBox)
	g.rotated = make([]int, gConst.NumBox)
	g.progress = -1
	return &g
}

func (gameState *GameState) ApplyAction(estimatedSize [][2]int, action *Action) {
	// エラー検出
	if action.boxId <= gameState.progress {
		fmt.Fprintf(os.Stderr, "ERROR: p must satisfy pi<pj (boxid=%d,progress=%d) .\n", action.boxId, gameState.progress)
		panic("")
	}
	if action.prevId >= 0 {
		if !gameState.placed[action.prevId] {
			fmt.Fprintf(os.Stderr, "ERROR: box %d is not placed yet.\n", action.prevId)
			panic("")
		}
	}
	if gameState.placed[action.boxId] {
		fmt.Fprintf(os.Stderr, "ERROR: box %d is already placed.\n", action.boxId)
		panic("")
	}
	xMin := 0
	if action.prevId >= 0 {
		xMin = gameState.positions[action.prevId][action.dir] + estimatedSize[action.prevId][gameState.rotated[action.prevId]^action.dir]
	}
	length := estimatedSize[action.boxId][action.rotate^action.dir]
	yMin := gameState.GetMinY(action.dir, xMin, estimatedSize, length)
	gameState.placed[action.boxId] = true
	gameState.positions[action.boxId][action.dir] = xMin
	gameState.positions[action.boxId][1^action.dir] = yMin
	gameState.rotated[action.boxId] = action.rotate
	gameState.progress = action.boxId
}

// 一から順番に埋めている前提で使えるrevert
func (gameState *GameState) Undo() {
	gameState.placed[gameState.progress] = false
	gameState.progress--
}

// 指定した座標方向に横長の棒を置くときどこに設置できるかを返す
func (gameState *GameState) GetMinY(dir int, xMin int, estimatedSizes [][2]int, length int) int {
	result := 0
	xMax := xMin + length
	for id := 0; id <= gameState.progress; id++ {
		if !gameState.placed[id] {
			continue
		}
		xMin2 := gameState.positions[id][dir]
		xMax2 := xMin2 + estimatedSizes[id][dir^gameState.rotated[id]]
		if xMax2 <= xMin || xMax <= xMin2 {
			// 衝突しない
			continue
		}
		// 衝突するのでresult更新
		yMax2 := gameState.positions[id][1^dir] + estimatedSizes[id][1^dir^gameState.rotated[id]]
		result = maxInt(result, yMax2)
	}
	return result
}

type TrueData struct {
	errors [][2]int
	sizes  [][2]int
}

func NewTrueData() *TrueData {
	td := TrueData{}
	td.errors = make([][2]int, 0, 400)
	td.sizes = make([][2]int, 0, 100)
	return &td
}

// 1ケース実行中において不変なものを保存する構造体
type GameConst struct {
	Random  *rand.Rand
	Scanner *bufio.Scanner

	NumBox                int
	SqrtNumBox            int
	NumQuery              int
	Sigma                 int
	InitialEstimatedSizes [][2]int
	EstimatedSigma        int

	bsPool sync.Pool

	TrueData *TrueData
}

func (g *GameConst) SetScanner(scanner *bufio.Scanner) {
	g.Scanner = scanner
}

func NewGameConst(runner *Runner) *GameConst {
	g := GameConst{}
	g.Random = rand.New(rand.NewSource(runner.randSeed))
	g.InitialEstimatedSizes = make([][2]int, 0, 100)
	g.TrueData = NewTrueData()
	return &g
}

func printActions(actions []*Action, comment bool) {
	fmt.Println(len(actions))
	for i := range actions {
		printAction(actions[i], comment)
	}
}
func printAction(action *Action, comment bool) {
	dirStr := "U"
	if action.dir == 1 {
		dirStr = "L"
	}
	fmt.Println(action.boxId, action.rotate, dirStr, action.prevId)
}

type Result struct {
	loopNum int
	score   int
}

type Runner struct {
	caseIndex int
	randSeed  int64
}

func NewRunner(caseIndex int, randseed int64) *Runner {
	runner := Runner{}
	runner.caseIndex = caseIndex
	runner.randSeed = randseed

	return &runner
}

func test(testCase int) Result {
	file := fmt.Sprintf("%04d", testCase)
	// runner := NewRunner(testCase, time.Now().UnixNano())
	runner := NewRunner(testCase, 0)
	result := runner.doGame(file)
	if DEBUG {
		fmt.Fprintf(os.Stderr, "score:%d, time:%fs (case=%04d)\n", result.score, float64(result.loopNum)/1000000, testCase)
	}
	return result
}

func parallelTester(prefix int, bests [TEST_NUM]Result) [TEST_NUM]Result {
	fmt.Fprintln(os.Stderr, "waiting...")
	time.Sleep(3 * time.Second)
	fmt.Fprintln(os.Stderr, "start!")

	// channelを作成
	results := [TEST_NUM]Result{}
	var wg sync.WaitGroup
	channels := make(chan struct{}, PARALLEL_NUM)

	wg.Add(TEST_NUM)
	// ケースを並行に実行
	for num := 0; num < TEST_NUM; num++ {
		channels <- struct{}{}
		go func(i int) {
			defer wg.Done()
			number := 0
			if REPEAT {
				number, _ = strconv.Atoi(FILE_NUMBER)
			} else {
				number = i + prefix
			}
			r := test(number)
			// }
			results[i] = r
			<-channels
		}(num)
	}
	wg.Wait()
	return results
}

func getBests() [TEST_NUM]Result {

	results := [TEST_NUM]Result{}

	scanner := getScanner(BEST_FILE)
	for caseIndex := 0; caseIndex < TEST_NUM; caseIndex++ {
		scanner.Scan()
		var caseNumber, score, loopNum int
		fmt.Sscan(scanner.Text(), &caseNumber, &score, &loopNum)
		results[caseIndex].score = score
		results[caseIndex].loopNum = loopNum

	}
	return results
}

func main() {
	if SUCCESSIVE && LOCAL_TEST {
		// ローカルでの複数実行モード

		timeStamp := time.Now().Unix()
		if RECORD_CODE {
			// 現コードを保存
			codeReadPath := "main.go"
			codePrintPath := fmt.Sprintf("%s\\code_%d-%d_%d.go", CODE_FILE, PREFIX, PREFIX+TEST_NUM-1, timeStamp)
			scanner := getScanner(codeReadPath)
			fp2, _ := os.Create(codePrintPath)
			loop := 0
			for {
				remains := scanner.Scan()
				if !remains {
					break
				}
				if loop == 0 {
					fmt.Fprintf(fp2, "// last local submission: %d\n", timeStamp)
				} else {
					fmt.Fprintln(fp2, scanner.Text())
				}
				loop++
			}
			fp2.Close()
		}

		bests := [TEST_NUM]Result{}
		if USE_RELATIVE_SCORE {
			bests = getBests()
		}
		// 複数ケースを並行で実行
		results := parallelTester(PREFIX, bests)

		// スコア・相対スコア計算
		sumScore := 0
		sumRelativeScore := 0.0
		minsScore := INF
		for i := 0; i < TEST_NUM; i++ {
			sumScore += results[i].score
			if USE_RELATIVE_SCORE {
				if MINIMIZE {
					if results[i].score != 0 {
						sumRelativeScore += math.Min(float64(bests[i].score)/float64(results[i].score), 1)
					} else {
						sumRelativeScore++
					}
				} else {
					if bests[i].score != 0 {
						sumRelativeScore += float64(results[i].score) / float64(bests[i].score)
					} else {
						sumRelativeScore++
					}
				}
			}
			if results[i].score == INF {
				fmt.Fprintf(os.Stderr, "WARN: case %d failed.\n", i)
			}
			minsScore = minInt(minsScore, results[i].score)
		}
		fmt.Println(sumRelativeScore)

		// 結果の出力
		fmt.Fprintf(os.Stderr, "\n---END---\n%d case finished.\n", TEST_NUM)
		fmt.Fprintln(os.Stderr, "sumScore:", sumScore)
		if USE_RELATIVE_SCORE {
			fmt.Fprintln(os.Stderr, "sumRelativeScore:", sumRelativeScore)
		}
		fmt.Fprintf(os.Stderr, "\n")
		for i := 0; i < TEST_NUM; i++ {
			fmt.Fprintf(os.Stderr, "%d ", results[i].score)
		}
		fmt.Fprintln(os.Stderr, "")

		fmt.Fprintf(os.Stderr, "// last local submission: %d\n", timeStamp)

		if !REPEAT {
			if RECORD {
				// 複数実行結果をファイルに保存
				path := fmt.Sprintf("%s\\%d-%d_%d.txt", RECORD_FOLDER, PREFIX, PREFIX+TEST_NUM-1, timeStamp)
				fp, _ := os.Create(path)
				fmt.Fprintf(fp, "(%d.%d.%d)%s\n%t\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_NAME, VERSION_VALID)
				for i := 0; i < TEST_NUM; i++ {
					fmt.Fprintf(fp, "%d %d %d\n", i, results[i].score, results[i].loopNum)
				}
				fp.Close()
			}
		} else {
			fmt.Fprintf(os.Stderr, "minScore: %d", minsScore)
		}
		fmt.Fprintf(os.Stderr, "\n")

	} else {
		// 単体ケースを実行
		caseIndex := 0
		if LOCAL_TEST {
			caseIndex, _ = strconv.Atoi(FILE_NUMBER)
		}
		runner := NewRunner(caseIndex, time.Now().UnixNano())
		result := runner.doGame(FILE_NUMBER)
		if LOCAL_TEST && DEBUG {
			fmt.Fprintf(os.Stderr, "score: %d\n", result.score)
		}
	}
}

type Action struct {
	boxId  int // 設置するbox
	prevId int // 設置基準のbox
	rotate int
	dir    int // 0: U 1: L
}

func NewAction() *Action {
	a := Action{}
	return &a
}

func (a *Action) Load(a2 *Action) {

}
func (a *Action) Copy() *Action {
	newa := NewAction()
	newa.Load(a)
	return newa
}

func getScore() float64 {
	return 0.0
}

func evaluate(gameState *GameState, gameConst *GameConst) float64 {
	value := 0.0
	return value
}

type Solver struct {
	start time.Time

	bs            *BeamSearch // 過去のbeamSearch
	forbiddenList [][]int

	history        [][]int // 各推定クエリでどちらの辺が採用されたか
	resultHistory  []int
	estimatedSizes [][][2]int // [sampleIndex][boxId][d]
}

func NewSolver(time time.Time, gameConst *GameConst) *Solver {
	s := Solver{}
	s.start = time
	s.bs = nil
	s.forbiddenList = make([][]int, 0)
	s.history = make([][]int, 0, gameConst.NumQuery)
	s.resultHistory = make([]int, 0, gameConst.NumQuery)
	s.estimatedSizes = make([][][2]int, SAMPLE_NUM)
	for i := range s.estimatedSizes {
		s.estimatedSizes[i] = make([][2]int, gameConst.NumBox)
		for j := range s.estimatedSizes[i] {
			for d := 0; d < 2; d++ {
				s.estimatedSizes[i][j][d] = gameConst.InitialEstimatedSizes[j][d]
				if i > 0 {
					s.estimatedSizes[i][j][d] += int(gameConst.Random.NormFloat64() * 1 * float64(gameConst.Sigma))
				}
			}
		}
	}
	return &s
}

func (s *Solver) MakeGuess(gameConst *GameConst) []*Action {
	maxAction := make([]*Action, 0, gameConst.NumBox)
	maxScore := 0.0
	_ = maxScore
	estimatedResult := make([][2]int, SAMPLE_NUM)
	bestvar := 0
	now := make([]int, gameConst.NumBox)
	for i := range now {
		now[i] = -1
	}
	for tryCount := 0; tryCount < 1000; tryCount++ {
		index := gameConst.Random.Intn(gameConst.NumBox)
		rotate := gameConst.Random.Intn(2)
		if now[index] != -1 {
			rotate = now[index] ^ 1
		}
		sqSum := [2]int{}
		sum := [2]int{}
		for sampleIndex := 0; sampleIndex < SAMPLE_NUM; sampleIndex++ {
			newW := estimatedResult[sampleIndex][0]
			newH := estimatedResult[sampleIndex][1]
			if index < gameConst.NumBox/2 {
				if now[index] != -1 {
					newW -= s.estimatedSizes[sampleIndex][index][now[index]]
				}
				newW += s.estimatedSizes[sampleIndex][index][rotate]
			} else {
				if now[index] != -1 {
					newH -= s.estimatedSizes[sampleIndex][index][1^now[index]]
				}
				newH += s.estimatedSizes[sampleIndex][index][1^rotate]
			}
			sum[0] += newW
			sqSum[0] += newW * newW
			sum[1] += newH
			sqSum[1] += newH * newH
		}
		sumVar := 0
		for d := 0; d < 2; d++ {
			variance := sqSum[d]/SAMPLE_NUM - (sum[d]/SAMPLE_NUM)*(sum[d]/SAMPLE_NUM)
			sumVar += variance
		}
		if sumVar >= bestvar || (index < gameConst.NumBox/2 && estimatedResult[0][0] < 200000) || (index < gameConst.NumBox/2 && estimatedResult[0][1] < 200000) {
			bestvar = sumVar
			for sampleIndex := 0; sampleIndex < SAMPLE_NUM; sampleIndex++ {
				if index < gameConst.NumBox/2 {
					if now[index] != -1 {
						estimatedResult[sampleIndex][0] -= s.estimatedSizes[sampleIndex][index][now[index]]
					}
					estimatedResult[sampleIndex][0] += s.estimatedSizes[sampleIndex][index][rotate]
				} else {
					if now[index] != -1 {
						estimatedResult[sampleIndex][1] -= s.estimatedSizes[sampleIndex][index][now[index]]
					}
					estimatedResult[sampleIndex][1] += s.estimatedSizes[sampleIndex][index][1^rotate]
				}
			}
			now[index] = rotate

			// fmt.Fprintln(os.Stderr, tryCount, index)
		}
	}
	// fmt.Fprintln(os.Stderr, math.Sqrt(float64(bestvar/2)))
	prev := -1
	for i := 0; i < gameConst.NumBox; i++ {
		if now[i] == -1 {
			continue
		}
		action := NewAction()
		action.boxId = i
		action.prevId = prev
		action.dir = 0
		action.rotate = now[i]
		maxAction = append(maxAction, action)
		if i < gameConst.NumBox/2 {
			prev = i
		}
	}

	/*
		prev := -1
		for i := 0; i < gameConst.NumBox; i++ {
			rotate := gameConst.Random.Intn(4)
			if rotate > 1 {
				continue
			}
			action := NewAction()
			action.boxId = i
			action.prevId = prev
			action.dir = 0
			action.rotate = rotate
			maxAction = append(maxAction, action)
			if i < gameConst.NumBox/2 {
				prev = i
			}
		}
	*/
	return maxAction
}

func (s *Solver) MakeGuess2(gameConst *GameConst) []*Action {
	maxAction := make([]*Action, 0, gameConst.NumBox)
	maxScore := 0.0
	_ = maxScore
	now := make([]int, gameConst.NumBox)
	for i := range now {
		now[i] = -1
	}
	preferredDir := make([]int, gameConst.NumBox)
	order := make([]int, gameConst.NumBox)
	for i := range order {
		order[i] = i
	}
	priority := make([]int, gameConst.NumBox)
	for i := range preferredDir {
		for d := 0; d < 2; d++ {
			p := 0
			for sampleIndex := 1; sampleIndex < SAMPLE_NUM; sampleIndex++ {
				p += absInt(s.estimatedSizes[sampleIndex][i][d] - s.estimatedSizes[0][i][d])
			}
			if p > priority[i] {
				priority[i] = p
				preferredDir[i] = d
			}
		}
	}
	// fmt.Fprintln(os.Stderr, priority)

	sort.Slice(order, func(i, j int) bool { return priority[order[i]] > priority[order[j]] })
	count1 := 0
	count2 := 0
	for _, i := range order {
		if i < gameConst.NumBox/2 {
			if count1 > gameConst.NumBox/4 {
				continue
			}
			now[i] = preferredDir[i]
		} else {
			if count2 > gameConst.NumBox/4 {
				continue
			}
			now[i] = preferredDir[i] ^ 1
		}
		if i < gameConst.NumBox/2 {
			count1++
		} else {
			count2++
		}
	}
	// fmt.Fprintln(os.Stderr, math.Sqrt(float64(bestvar/2)))
	prev := -1
	for i := 0; i < gameConst.NumBox; i++ {
		if now[i] == -1 {
			continue
		}
		action := NewAction()
		action.boxId = i
		action.prevId = prev
		action.dir = 0
		action.rotate = now[i]
		maxAction = append(maxAction, action)
		if i < gameConst.NumBox/2 {
			prev = i
		}
	}
	return maxAction
}
func (solver *Solver) Solve(gameConst *GameConst, estimatedMinScore int, duration int) ([]*Action, int) {
	start := time.Now()
	_ = start
	maxAction := make([]*Action, 0, gameConst.NumBox)
	estimatedScore := 0
	_ = estimatedScore

	gameStates := make([]*GameState, 0, len(solver.estimatedSizes))
	for _ = range solver.estimatedSizes {
		gameStates = append(gameStates, NewGameState(gameConst))
	}

	// 貪欲を書くならここ

	// ランダム
	// for i := 0; i < gameConst.NumBox; i++ {
	// 	action := NewAction()
	// 	action.boxId = i
	// 	// action.prevId = i - 1
	// 	action.prevId = gameConst.Random.Intn(i+1) - 1
	// 	// action.dir = 0
	// 	action.dir = gameConst.Random.Intn(2)
	// 	// action.rotate = 0
	// 	action.rotate = gameConst.Random.Intn(2)
	// 	maxAction = append(maxAction, action)
	// }

	sumArea := 0
	for s := range solver.estimatedSizes {
		for i := 0; i < gameConst.NumBox; i++ {
			sumArea += solver.estimatedSizes[s][i][0] * solver.estimatedSizes[s][i][1]
		}
	}
	sumArea /= len(solver.estimatedSizes)

	border := int(math.Sqrt(float64(sumArea)) * (1.03 + gameConst.Random.Float64()*0.03))

	/*count := 0
	isCandidate := make([]bool, gameConst.NumBox)
	for count < gameConst.NumBox {
		rotate := gameConst.Random.Intn(2)
		bestAction := NewAction()
		minScore := 10000000000
		for prev := -1; prev < count; prev++ {
			d := 0
			if prev >= 0 && !isCandidate[prev] {
				continue
			}
			action := NewAction()
			action.boxId = count
			action.prevId = prev
			action.dir = d
			action.rotate = rotate
			sumScore := 0
			for j := range solver.estimatedSizes {
				gameStates[j].ApplyAction(solver.estimatedSizes[j], action)
				score, sumW, _, _ := calcScore(gameStates[j], gameConst, solver.estimatedSizes[j], border)
				if sumW > border {
					score += 1000000
				}
				// sumScore = maxInt(sumScore, score)
				sumScore += score
				gameStates[j].Undo()
			}
			if sumScore <= minScore {
				minScore = sumScore
				bestAction = action
			}
		}
		for j := range solver.estimatedSizes {
			gameStates[j].ApplyAction(solver.estimatedSizes[j], bestAction)
		}
		maxAction = append(maxAction, bestAction)
		estimatedScore = minScore
		if bestAction.prevId >= 0 {
			isCandidate[bestAction.prevId] = false
		}
		isCandidate[bestAction.boxId] = true
		count++
	}*/
	minNeedeSpace := INF
	for i := range solver.estimatedSizes[0] {
		for d := range solver.estimatedSizes[0][i] {
			minNeedeSpace = minInt(minNeedeSpace, solver.estimatedSizes[0][i][d])
		}
	}

	if solver.bs == nil || solver.bs.bestIndex != 0 || solver.bs.bestIndex == 0 {
		if solver.bs != nil {
			solver.bs.ReleaseBeamStates(gameConst)
		}
		bs := NewBeamSearch()
		solver.bs = bs
		weight := math.Pow(float64(gameConst.NumBox), 1.4) * (1.3 - 0.3*float64(sumArea/gameConst.NumBox)/70000/70000)
		width := int(1000000 / weight)
		if ADAPTIVE {
			width = width * duration / (350000 * TIME_RATE)
			if LOCAL_TEST && !SUCCESSIVE {
				fmt.Fprintln(os.Stderr, "width:", width)
			}
			width = maxInt(width, 200)
		}
		// fmt.Fprintln(os.Stderr, width)
		bs.Init(width, gameConst.NumBox, minNeedeSpace, solver.start)
		bState := NewBeamSearchState(gameConst.NumBox)
		bs.Run(bState, solver.estimatedSizes[0], gameConst, border, estimatedMinScore)
		// fmt.Fprintln(os.Stderr, bs.effectiveStates[bs.depth][0].uncertainty)
	} else {
		for {
			solver.bs.bestIndex += 1
			if solver.bs.effectiveProcedures[solver.bs.depth][solver.bs.bestIndex-1].prevState == solver.bs.effectiveProcedures[solver.bs.depth][solver.bs.bestIndex].prevState {
				continue
			}
			break
		}
		solver.bs.nowDepth = 0
	}

	// for {
	// 	solver.bs.nowDepth = 0
	// 	ok := true
	// 	bstate := solver.bs.effectiveStates[solver.bs.depth][solver.bs.bestIndex]
	// 	for index := range solver.forbiddenList {
	// 		same := true
	// 		for i := range solver.forbiddenList[index] {
	// 			if solver.forbiddenList[index][i] != bstate.prevs[i] {
	// 				same = false
	// 				break
	// 			}
	// 		}
	// 		if same {
	// 			ok = false
	// 			break
	// 		}
	// 	}
	// 	if ok {
	// 		break
	// 	}
	// 	solver.bs.bestIndex++
	//
	// }
	// fmt.Fprintln(os.Stderr, solver.bs.bestIndex)

	// bState := gameConst.bsPool.Get().(*BeamSearchState)
	for {
		maxAction = []*Action{}
		for i := 0; i < solver.bs.depth; i++ {
			beamAction, bestBeamState := solver.bs.GetAction()
			// beamAction, bestBeamState := solver.bs.Run(bState, gameConst.TrueData.sizes, gameConst, border, estimatedMinScore)
			if i == 0 && !SUCCESSIVE {
				fmt.Println("# estimated_h", bestBeamState.height)
				// for j := 0; j < 20; j++ {
				// 	fmt.Fprintln(os.Stderr, solver.bs.effectiveProcedures[solver.bs.depth][j].beamValue, solver.bs.effectiveStates[solver.bs.depth][j].prevs[:20])
				// }
				bestBeamState.borderline.DebugPrint()
			}
			action := NewAction()
			action.dir = 0
			action.boxId = i
			action.rotate = beamAction & 1
			action.prevId = beamAction >> 1
			if action.prevId == i {
				action.prevId = -1
			}
			maxAction = append(maxAction, action)
		}
		if solver.bs.depth < gameConst.NumBox {
			break
		}
		// 重複チェック
		ok := true
		for index := range solver.forbiddenList {
			same := true
			for i := range solver.forbiddenList[index] {
				if solver.forbiddenList[index][i] != maxAction[i].prevId<<1+maxAction[i].rotate {
					same = false
					break
				}
			}
			if same {
				ok = false
				break
			}
		}
		if ok {
			// 重複なしなので決定
			break
		}
		if solver.bs.bestIndex == solver.bs.numEffectiveStates[solver.bs.depth]-1 {
			// 最終候補のため決定
			break
		}
		solver.bs.nowDepth = 0
		solver.bs.bestIndex++
		// fmt.Fprintln(os.Stderr, "!!!!!!!!!!!!!!!!!!!!!!!")
	}

	if len(maxAction) == gameConst.NumBox {
		solver.forbiddenList = append(solver.forbiddenList, []int{})
		for i := 0; i < gameConst.NumBox; i++ {
			solver.forbiddenList[len(solver.forbiddenList)-1] = append(solver.forbiddenList[len(solver.forbiddenList)-1], maxAction[i].prevId<<1+maxAction[i].rotate)
		}
	}
	// bs.ReleaseBeamStates(gameConst)

	// 貪欲ここまで
	if LOCAL_TEST && !SUCCESSIVE {
		fmt.Fprintln(os.Stderr, estimatedScore)
	}

	score := 0

	return maxAction, score
}

func (runner *Runner) doGame(file string) Result {
	result := Result{}
	scanner := getScanner(FILE + "\\" + file + ".txt")

	// 初期値設定処理
	start := time.Now()

	gameConst := NewGameConst(runner)
	gameConst.SetScanner(scanner)
	// データ取得
	getInputs(gameConst)
	// sumArea := 0
	// for i := range gameConst.TrueData.sizes {
	// 	sumArea += gameConst.TrueData.sizes[i][0] * gameConst.TrueData.sizes[i][1]
	// }
	// result.score = int(math.Sqrt(float64(sumArea-1)))*2 + 1
	// return result

	solver := NewSolver(start, gameConst)
	// 初期値設定処理　ここまで

	// tryNum := gameConst.NumQuery / 2
	tryNum := 12
	minScore := INF
	estimatedMinScore := INF
	durationRate := 0.0
	stability := make([][2]int, gameConst.NumBox)
	_ = stability

	for queryCount := 0; queryCount < gameConst.NumQuery; queryCount++ {
		var actions []*Action
		guess := false

		if queryCount < gameConst.NumQuery-tryNum {
			guess = true
		}
		// if queryCount == gameConst.NumQuery-tryNum {
		// 	start = time.Now()
		// }
		if guess {
			actions = solver.MakeGuess(gameConst)
			for i := range actions {
				stability[actions[i].boxId][actions[i].rotate]++
			}
		} else {
			duration := int(TIME_LIMIT-time.Since(start).Microseconds()) / (gameConst.NumQuery - queryCount)
			if durationRate != 0 {
				duration = int(float64(duration) * durationRate)
			}
			s := time.Now()
			actions, _ = solver.Solve(gameConst, estimatedMinScore, duration)
			if durationRate == 0 && time.Since(s).Microseconds() != 0 {
				durationRate = float64(duration) / float64(time.Since(s).Microseconds())
			}
		}

		// アクションを出力
		if !LOCAL_TEST || (!SUCCESSIVE && PRINT_ACTION) {
			printActions(actions, false)
		}
		w := 0
		h := 0
		_ = w
		_ = h
		if LOCAL_TEST {
			// 結果を生成
			w, h = calcResult(gameConst.TrueData.sizes, actions, queryCount, gameConst, true)
		} else {
			// 結果を取得
			w, h = getResult(gameConst)
		}
		if guess {

			// 横方向
			lenhistory := len(solver.history)
			solver.history = append(solver.history, make([]int, gameConst.NumBox))
			for i := range solver.history[lenhistory] {
				solver.history[lenhistory][i] = -1
			}
			maxIndex := -1
			for i := range actions {
				id := actions[i].boxId
				if i == len(actions)-1 || actions[i].boxId >= gameConst.NumBox/2 {
					if maxIndex == -1 || gameConst.InitialEstimatedSizes[id][actions[i].rotate] > gameConst.InitialEstimatedSizes[actions[maxIndex].boxId][actions[maxIndex].rotate] {
						maxIndex = i
					}
					continue
				}
				rotate := actions[i].rotate
				solver.history[lenhistory][id] = rotate
			}
			solver.history[lenhistory][actions[maxIndex].boxId] = actions[maxIndex].rotate
			solver.resultHistory = append(solver.resultHistory, 0)
			solver.resultHistory[lenhistory] = w
			// 縦方向
			lenhistory = len(solver.history)
			solver.history = append(solver.history, make([]int, gameConst.NumBox))
			for i := range solver.history[lenhistory] {
				solver.history[lenhistory][i] = -1
			}
			for i := range actions {
				id := actions[i].boxId
				if i == len(actions)-1 || actions[i].boxId >= gameConst.NumBox/2 {

				} else {
					continue
				}
				rotate := actions[i].rotate ^ 1
				solver.history[lenhistory][id] = rotate
			}
			solver.resultHistory = append(solver.resultHistory, 0)
			solver.resultHistory[lenhistory] = h
			// 予想修正
			for s := 1; s < SAMPLE_NUM; s++ {
				for i := range solver.estimatedSizes[0] {
					for d := 0; d < 2; d++ {
						sizeError := int(gameConst.Random.NormFloat64() * float64(gameConst.Sigma) /* / math.Sqrt(float64(stability[i][d]+1))*/)
						if sizeError > 0 {
							sizeError += gameConst.Sigma
						} else {
							sizeError -= gameConst.Sigma
						}
						// fmt.Fprintln(os.Stderr, sizeError)
						solver.estimatedSizes[s][i][d] = solver.estimatedSizes[0][i][d] + sizeError
					}
				}
			}
			for l := 0; l < 1; l++ {
				for q := range solver.history {
					for s := range solver.estimatedSizes {

						estimatedSumW := 0
						numBox := 0
						for i := 0; i < gameConst.NumBox; i++ {
							if solver.history[q][i] != -1 {
								estimatedSumW += solver.estimatedSizes[s][i][solver.history[q][i]]
								numBox++
							}
						}
						// fmt.Fprintln(os.Stderr, solver.resultHistory[q], estimatedSumW)
						diff := solver.resultHistory[q] - estimatedSumW
						diffParBox := diff * absInt(diff) / (absInt(diff) + 2*gameConst.Sigma) / numBox
						_ = diffParBox

						for i := 0; i < gameConst.NumBox; i++ {
							if solver.history[q][i] != -1 {
								solver.estimatedSizes[s][i][solver.history[q][i]] += diffParBox
								// solver.estimatedSizes[s][i][solver.history[q][i]] += diff * absInt(diff) * solver.estimatedSizes[s][i][solver.history[q][i]] / estimatedSumW / (absInt(diff) + 2*gameConst.Sigma)
							}
						}
						if LOCAL_TEST && !SUCCESSIVE && s == 0 && q == len(solver.history)-1 {
							sumdiff := 0
							sumdiff2 := 0
							sumdiff3 := 0
							maxDiff := 0
							for i := range gameConst.TrueData.sizes {
								for d := 0; d < 2; d++ {
									sumdiff += absInt(gameConst.TrueData.sizes[i][d] - solver.estimatedSizes[s][i][d])
									maxDiff = maxInt(maxDiff, absInt(gameConst.TrueData.sizes[i][d]-solver.estimatedSizes[s][i][d]))
									if i < gameConst.NumBox/2 {
										sumdiff2 += absInt(gameConst.TrueData.sizes[i][d] - solver.estimatedSizes[s][i][d])
									} else {
										sumdiff3 += absInt(gameConst.TrueData.sizes[i][d] - solver.estimatedSizes[s][i][d])
									}
									// if queryCount == gameConst.NumQuery-11 {
									// 	fmt.Fprintln(os.Stderr, gameConst.TrueData.sizes[i][d], solver.estimatedSizes[s][i][d], gameConst.InitialEstimatedSizes[i][d])
									// }
								}
							}

							fmt.Println("#prev_diff:", diff)
							fmt.Fprintln(os.Stderr, maxDiff, sumdiff, sumdiff2, sumdiff3, "(", diff, gameConst.TrueData.errors[queryCount][0], ")")
						}
					}
				}
			}
			if queryCount == gameConst.NumQuery-tryNum-1 {
				for id := 0; id < gameConst.NumBox; id++ {
					for dir := 0; dir < 2; dir++ {
						solver.history = append(solver.history, make([]int, gameConst.NumBox))
						for i := range solver.history[len(solver.history)-1] {
							solver.history[len(solver.history)-1][i] = -1
						}
						solver.history[len(solver.history)-1][id] = dir
						solver.resultHistory = append(solver.resultHistory, 0)
						solver.resultHistory[len(solver.resultHistory)-1] = gameConst.InitialEstimatedSizes[id][dir]
					}
				}
				for s := 0; s < SAMPLE_NUM; s++ {
					for i := range solver.estimatedSizes[s] {
						for d := range solver.estimatedSizes[s][i] {
							solver.estimatedSizes[s][i][d] = maxInt(solver.estimatedSizes[s][i][d], 10000+gameConst.EstimatedSigma/2)
							solver.estimatedSizes[s][i][d] = minInt(solver.estimatedSizes[s][i][d], 100000-gameConst.EstimatedSigma/2)
						}
					}
				}
				// SA
				// fmt.Fprintln(os.Stderr, stability)
				sa := NewSA()
				// startTemp := math.Sqrt(float64(len(solver.history))) * float64((gameConst.Sigma * gameConst.Sigma / 100))
				// endTemp := float64(1 * (gameConst.Sigma * gameConst.Sigma / 1000))
				startTemp := math.Sqrt(float64(len(solver.history))) * 0.01
				endTemp := 0.0001
				// startTemp := endTemp
				best := NewSimulatedAnnealingPlan(gameConst.NumBox, len(solver.history))
				for i := 0; i < gameConst.NumBox; i++ {
					for d := 0; d < 2; d++ {
						best.size[i][d] = solver.estimatedSizes[0][i][d]
					}

				}
				for q := range solver.history {
					estimatedSumW := 0
					for i := 0; i < gameConst.NumBox; i++ {
						if solver.history[q][i] != -1 {
							estimatedSumW += solver.estimatedSizes[0][i][solver.history[q][i]]
						}
					}
					diff := estimatedSumW - solver.resultHistory[q]
					best.difference[q] = diff
				}
				bestScore := best.Evaluate(gameConst)
				sa.Init(200*1000, startTemp, endTemp, best, bestScore)
				if LOCAL_TEST && !SUCCESSIVE {
					fmt.Fprintln(os.Stderr, "old_best:", bestScore)
				}
				sa.Run(gameConst, solver.history)
				for i := 0; i < gameConst.NumBox; i++ {
					for d := 0; d < 2; d++ {
						solver.estimatedSizes[0][i][d] = sa.best.size[i][d]
					}
				}
				// for i := 0; i < gameConst.NumBox; i++ {
				// 	for d := 0; d < 2; d++ {
				// 		solver.estimatedSizes[0][i][d] = gameConst.TrueData.sizes[i][d]
				// 	}
				// }

				if LOCAL_TEST && !SUCCESSIVE {

					sumdiff := 0
					sumdiff2 := 0
					sumdiff3 := 0
					maxDiff := 0
					for i := range gameConst.TrueData.sizes {
						for d := 0; d < 2; d++ {
							sumdiff += absInt(gameConst.TrueData.sizes[i][d] - solver.estimatedSizes[0][i][d])
							maxDiff = maxInt(maxDiff, absInt(gameConst.TrueData.sizes[i][d]-solver.estimatedSizes[0][i][d]))

							if i < gameConst.NumBox/2 {
								sumdiff2 += absInt(gameConst.TrueData.sizes[i][d] - solver.estimatedSizes[0][i][d])
							} else {
								sumdiff3 += absInt(gameConst.TrueData.sizes[i][d] - solver.estimatedSizes[0][i][d])
							}
							// if queryCount == gameConst.NumQuery-11 {
							// 	fmt.Fprintln(os.Stderr, gameConst.TrueData.sizes[i][d], solver.estimatedSizes[s][i][d], gameConst.InitialEstimatedSizes[i][d])
							// }
						}
					}
					fmt.Fprintln(os.Stderr, maxDiff, sumdiff, sumdiff2, sumdiff3)
				}
			}
		} else {
			// 大きな齟齬が発生していないか確認
			estimatedW, estimatedH := calcResult(solver.estimatedSizes[0], actions, queryCount, gameConst, false)
			if w+h <= estimatedW+estimatedH+gameConst.Sigma {
				// おおむね当たっていたとする
				// fmt.Fprintln(os.Stderr, "success!!!", runner.caseIndex, estimatedW+estimatedH+gameConst.Sigma, w+h)
				if estimatedW+estimatedH < estimatedMinScore {
					estimatedMinScore = estimatedW + estimatedH

				}
				gameConst.EstimatedSigma = int(float64(gameConst.EstimatedSigma) * 0.9)
			} else {
				// fmt.Fprintln(os.Stderr, "failure...")
			}

		}

		if LOCAL_TEST {
			// 実際のサイズで積み込む
			gameState := NewGameState(gameConst)
			for i := range actions {
				gameState.ApplyAction(gameConst.TrueData.sizes, actions[i])
			}
			// fmt.Fprintln(os.Stderr, gameState.positions)
			// スコア計算
			score, _, _, _ := calcScore(gameState, gameConst, gameConst.TrueData.sizes, 0)
			minScore = minInt(minScore, score)
		}
	}
	duration := time.Since(start).Microseconds()
	result.loopNum = int(duration)

	/*dll := NewDoublyLinkedList(50)
	for i := 0; i < 50; i++ {
		r1 := gameConst.Random.Intn(100)
		r2 := gameConst.Random.Intn(100)
		r3 := r2 + gameConst.Random.Intn(20) + 1
		diff := dll.UpdateHeights(r1, r2, r3)
		fmt.Fprintln(os.Stderr, r1, r2, r3, "diff =", diff)
	}

	for i := 0; i < 5; i++ {
		r2 := gameConst.Random.Intn(100)
		r3 := r2 + gameConst.Random.Intn(20) + 1
		height := dll.GetHeight(r2, r3)
		fmt.Fprintln(os.Stderr, "get height", r2, r3, "->", height)
	}
	dll.DebugPrint()
	// dll.UpdateHeights(3, 10, 50)
	// dll.UpdateHeights(5, 2, 12)
	// dll.UpdateHeights(6, 12, 24)
	// dll.UpdateHeights(4, 30, 65)
	// dll.UpdateHeights(8, 15, 33)*/

	result.score = minScore

	return result
}

func calcResult(estimatedSizes [][2]int, actions []*Action, queryCount int, gameConst *GameConst, queryResult bool) (int, int) {
	w := 0
	h := 0
	gameState := NewGameState(gameConst)
	for i := range actions {
		gameState.ApplyAction(estimatedSizes, actions[i])
	}
	for i := 0; i < gameConst.NumBox; i++ {
		w = maxInt(w, gameState.positions[i][0]+estimatedSizes[i][gameState.rotated[i]])
		h = maxInt(h, gameState.positions[i][1]+estimatedSizes[i][1^gameState.rotated[i]])
	}
	if queryResult {
		w += gameConst.TrueData.errors[queryCount][0]
		h += gameConst.TrueData.errors[queryCount][1]
	}
	return w, h
}

func getResult(gameConst *GameConst) (int, int) {
	gameConst.Scanner.Scan()
	splitedInput := strings.Split(gameConst.Scanner.Text(), " ")
	w, _ := strconv.Atoi(splitedInput[0])
	h, _ := strconv.Atoi(splitedInput[1])
	return w, h
}

func calcScore(gameState *GameState, gameConst *GameConst, estimatedSizes [][2]int, borderX int) (int, int, int, int) {
	score := 0
	maxX := 0
	maxY := 0
	penalty := 0
	for id := 0; id < gameConst.NumBox; id++ {
		// 未設置ならペナルティ
		if !gameState.placed[id] {
			penalty += estimatedSizes[id][0]
			penalty += estimatedSizes[id][1]
		}
		maxX = maxInt(maxX, gameState.positions[id][0]+estimatedSizes[id][0^gameState.rotated[id]])
		maxY = maxInt(maxY, gameState.positions[id][1]+estimatedSizes[id][1^gameState.rotated[id]])
	}
	maxX = maxInt(maxX, borderX)
	score = maxX + maxY + penalty
	return score, maxX, maxY, penalty
}

type BeamProcedure struct {
	prevState  int     // 遷移元の順位
	prevAction int     // 遷移元からここに来るために必要なAction
	beamValue  float64 // この状態の評価値
	randValue  float64
}

func NewBeamProcedure() *BeamProcedure {
	bp := BeamProcedure{}
	return &bp
}

func (bp *BeamProcedure) Copy() *BeamProcedure {
	newbp := NewBeamProcedure()
	newbp.Load(bp)
	return newbp
}

func (bp *BeamProcedure) Load(bp2 *BeamProcedure) {
	bp.prevState = bp2.prevState
	bp.prevAction = bp2.prevAction
	bp.beamValue = bp2.beamValue
	bp.randValue = bp2.randValue
}

// 盤面情報などを扱う構造体
type BeamSearchState struct {
	// ここに盤面の情報を格納する変数
	rightEnds     []int // 各boxの右端座標
	uncertainty   []int // 各boxの不確実性
	height        int   // height
	width         int
	progress      int
	borderline    *DoublyLinkedList // 一番下を表す線
	sumPlacedArea int
	sumUsedArea   int
	//

}

// NewBeamSearchState() 新しい状態を生成する関数
func NewBeamSearchState(numBox int) *BeamSearchState {
	g := &BeamSearchState{}
	g.rightEnds = make([]int, numBox)
	g.uncertainty = make([]int, numBox)
	g.borderline = NewDoublyLinkedList(numBox)
	return g
}

// Copy() Stateをコピーする関数
func (g *BeamSearchState) Copy() *BeamSearchState {
	newg := NewBeamSearchState(len(g.rightEnds))
	newg.Load(g)
	return newg
}

// Load() 既存のStateを上書きする関数 少し危険
func (g *BeamSearchState) Load(gameState *BeamSearchState) {

	g.progress = gameState.progress
	for i := 0; i < gameState.progress; i++ {
		g.rightEnds[i] = gameState.rightEnds[i]
	}
	for i := 0; i < gameState.progress; i++ {
		g.uncertainty[i] = gameState.uncertainty[i]
	}
	g.height = gameState.height
	g.width = gameState.width
	g.borderline.Load(gameState.borderline)
	g.sumPlacedArea = gameState.sumPlacedArea
	g.sumUsedArea = gameState.sumUsedArea

}

// 可能な行動か判定する関数
func (g *BeamSearchState) IsValid(action int, estimatedSizes [][2]int, gameConst *GameConst, border int, minNeededSpace int) bool {
	w2 := 0
	if action>>1 < g.progress {
		w2 = g.rightEnds[action>>1]
	} else {
		w2 = 0

	}
	w2 += estimatedSizes[g.progress][action&1]
	ok := w2 <= border
	if ok {
		if g.progress < gameConst.NumBox-10 {
			prevId := action >> 1
			rotate := action & 1
			uncertainty := 1
			if prevId < g.progress {
				uncertainty += g.uncertainty[prevId]
			}
			additionalspace := int(1 * math.Sqrt(float64(uncertainty)) * float64(gameConst.EstimatedSigma))
			if w2+minNeededSpace+additionalspace < border {
				for i := 0; i < g.progress; i++ {
					if i == g.progress {
						continue
					}
					if i == prevId {
						continue
					}
					if g.rightEnds[i] == -1 {
						continue
					}
					rightend := absInt(g.rightEnds[i])
					if rightend <= w2+2*additionalspace && rightend > w2-minInt(estimatedSizes[g.progress][rotate], 2*additionalspace) {
						ok = false
						break

					}
				}
			}
		}
	}

	return ok
}

// 行動を適用する関数
func (g *BeamSearchState) ApplyAction(action int, estimatedSizes [][2]int, gameConst *GameConst, minNeededSpace int, border int) {
	// 設置される高さを計算
	rMin := 0
	prevId := action >> 1
	rotate := action & 1
	if prevId < g.progress {
		rMin = g.rightEnds[prevId]
	}
	g.uncertainty[g.progress] = 1
	if prevId < g.progress {
		g.uncertainty[g.progress] += g.uncertainty[prevId]
	}
	rMax := rMin + estimatedSizes[g.progress][rotate]
	additionalspace := int(1 * math.Sqrt(float64(g.uncertainty[g.progress])) * float64(gameConst.EstimatedSigma))
	// additionalspace := int(1 * float64(gameConst.EstimatedSigma))
	h := g.borderline.GetHeight(rMin, rMax+additionalspace)
	loss := g.borderline.GetLoss(h, rMin, rMax)

	_ = loss
	estimatedHeight := h + estimatedSizes[g.progress][1^rotate]
	loss2 := g.borderline.GetAdditionalLoss(estimatedHeight, rMin, rMax, additionalspace, true, border)
	diff := g.borderline.UpdateHeights(estimatedHeight, rMin, rMax)
	_ = diff
	g.sumPlacedArea += estimatedSizes[g.progress][rotate] * estimatedSizes[g.progress][1^rotate]
	g.sumUsedArea += diff + loss2

	additionalspace = int(2 * math.Sqrt(float64(g.uncertainty[g.progress])) * float64(gameConst.EstimatedSigma))
	// additionalspace = int(3 * float64(gameConst.EstimatedSigma))
	for i := range g.rightEnds {
		rightEnd := absInt(g.rightEnds[i])
		// additionalspace := 2 * gameConst.EstimatedSigma
		if rightEnd > rMin && rightEnd < rMax+additionalspace {
			/*if i == action.prevId {
				h, _ := g.borderline.GetHeight(rMin-10, rMin)
				if h > estimatedHeight+100000 {
					continue
				}
			}*/
			g.rightEnds[i] = -1
		}
	}
	if prevId != g.progress && g.rightEnds[prevId] >= 0 && g.progress <= gameConst.NumBox-2 {
		h2 := g.borderline.GetHeight(rMin-1, rMin)
		if h2 > estimatedHeight+30000 {
			g.rightEnds[prevId] *= -1
		} else {
			g.rightEnds[prevId] = -1
		}
	}
	if g.progress == gameConst.NumBox-2 {
		for i := range g.rightEnds {
			if g.rightEnds[i] != -1 {
				g.rightEnds[i] = absInt(g.rightEnds[i])
			}
		}
	}
	g.rightEnds[g.progress] = rMax
	if rMax+minNeededSpace >= border {
		g.rightEnds[g.progress] = -1
	}
	g.height = maxInt(g.height, estimatedHeight)
	g.width = maxInt(g.width, rMax)

	g.progress++

}

// ビームサーチを扱う構造体
type BeamSearch struct {
	width               int                  // ビームサーチの幅
	depth               int                  // ビームサーチの深さ(終了までにかかりそうなAction数)
	nowDepth            int                  // 現在該当する深さ 計算が完了している時に使用される
	calculated          bool                 // 計算完了しているか
	effectiveStates     [][]*BeamSearchState // [depth][width]*BeamSearchState　有力候補を管理する変数
	effectiveProcedures [][]*BeamProcedure   // [depth][width]*BeamSearchState　有力候補の遷移を管理する変数
	numEffectiveStates  []int                // 各深さでの有力候補数
	bestIndex           int

	minNeededSpace int // 埋め込むのに最低限必要な幅

	start time.Time
}

func NewBeamSearch() *BeamSearch {
	return &BeamSearch{}
}

func (bs *BeamSearch) Init(width int, depth int, minNeededSpace int, start time.Time) {
	bs.start = start
	bs.width = width
	bs.depth = depth
	bs.nowDepth = 0
	bs.calculated = false
	bs.effectiveStates = make([][]*BeamSearchState, depth+1)
	for i := range bs.effectiveStates {
		// 最初に列を用意しているが幅を動的に変えたい場合は書き換えを検討
		bs.effectiveStates[i] = make([]*BeamSearchState, width)
	}
	bs.effectiveProcedures = make([][]*BeamProcedure, depth+1)
	for i := range bs.effectiveProcedures {
		// 最初に列を用意しているが幅を動的に変えたい場合は書き換えを検討
		bs.effectiveProcedures[i] = make([]*BeamProcedure, width)
	}
	bs.numEffectiveStates = make([]int, depth+1)
	bs.bestIndex = 0
	bs.minNeededSpace = minNeededSpace
}

// Stateに対し評価値を返す関数 速度を意識する場合は前回からの差分を検討したほうが良い
func (bs *BeamSearch) Evaluate(gameState *BeamSearchState, gameConst *GameConst, border int) float64 {
	// value := -float64(gameState.height)
	value := float64(gameState.sumPlacedArea - gameState.sumUsedArea)
	// value := 0.0
	// value -= math.Max(0, float64((gameState.height)))
	value -= math.Max(0, float64((gameState.height-border/2)*border)*0.5)
	// w:=0
	// for i:=range gameState.lineWidth{
	// 	w=maxInt(w,gameState.lineWidth[i])
	// }
	// value-=float64(w)
	return value
}

func (bs *BeamSearch) EasyEvaluateBefore(gameState *BeamSearchState, estimatedSizes [][2]int, action int, gameConst *GameConst, border int, estimatedMinScore int) float64 {
	rMin := 0
	prevId := action >> 1
	rotate := action & 1
	if prevId < gameState.progress {
		rMin = gameState.rightEnds[prevId]
	}
	rMax := rMin + estimatedSizes[gameState.progress][rotate]
	uncertainty := 1
	if prevId < gameState.progress {
		uncertainty += gameState.uncertainty[prevId]
	}
	additionalspace := int(1 * math.Sqrt(float64(uncertainty)) * float64(gameConst.EstimatedSigma))
	// additionalspace := gameConst.EstimatedSigma
	height := gameState.borderline.GetHeight(rMin, rMax+additionalspace)
	height += estimatedSizes[gameState.progress][1^rotate]
	mHeight := maxInt(gameState.height, height)
	mWidth := maxInt(gameState.width, rMax)
	return -float64(mHeight + mWidth)
}

func (bs *BeamSearch) EvaluateBefore(gameState *BeamSearchState, estimatedSizes [][2]int, action int, gameConst *GameConst, border int, estimatedMinScore int) float64 {
	// value := -float64(gameState.height)
	rMin := 0
	prevId := action >> 1
	rotate := action & 1
	if prevId < gameState.progress {
		rMin = gameState.rightEnds[prevId]
	}
	rMax := rMin + estimatedSizes[gameState.progress][rotate]
	uncertainty := 1
	if prevId < gameState.progress {
		uncertainty += gameState.uncertainty[prevId]
	}
	additionalspace := int(1 * math.Sqrt(float64(uncertainty)) * float64(gameConst.EstimatedSigma))
	// additionalspace := gameConst.EstimatedSigma
	height := gameState.borderline.GetHeight(rMin, rMax+additionalspace)
	loss := gameState.borderline.GetLoss(height, rMin, rMax)
	height += estimatedSizes[gameState.progress][1^rotate]
	loss2 := gameState.borderline.GetAdditionalLoss(height, rMin, rMax, additionalspace, false, border)
	value := float64(gameState.sumPlacedArea)
	value -= float64(gameState.sumUsedArea)
	value -= float64(loss)
	value -= float64(loss2)
	// height2, _ := gameState.borderline.GetHeight(0, rMax +gameConst.Sigma)
	// value -= float64((height2 - height) * border / 5)
	// value := 0.0
	// value -= math.Max(0, float64((gameState.height)))
	// value -= math.Max(0, float64((maxInt(gameState.height, height)-border/2)*border)*0.5)
	value -= math.Max(0, float64((maxInt(gameState.height, height)-(border-150000))*int(1500*float64(gameConst.NumBox))))
	mHeight := maxInt(gameState.height, height)
	mWidth := maxInt(gameState.width, rMax)
	if mHeight+mWidth > estimatedMinScore || mHeight+mWidth > border*border*11/10 {
		value -= float64(100000 * 100000)
	}
	return value
}

func (bs *BeamSearch) EvaluateDiff(gameState *BeamSearchState, action int, gameConst *GameConst) float64 {
	return 0.0
}

func (bs *BeamSearch) Run(gameState *BeamSearchState, estimatedSizes [][2]int, gameConst *GameConst, border int, estimatedMinScore int) {
	// 既に計算済みならいきなり復元パートに行ってよい
	if !bs.calculated {
		// 計算
		bsQueue := NewBeamSearchQueue(bs.width)
		bsQueue.Init()
		// 最初の深さは自明な初期状態のみ
		bsQueue.Push(NewBeamProcedure())
		for d := 0; d < bs.depth+1; d++ {
			// Queueの上位width個分だけ取り出す
			bsQueue.Sort()
			for i := 0; i < bsQueue.Len(); i++ {
				bsQueue.items[i].beamValue -= bsQueue.items[i].randValue
			}
			// bsQueue.Sort()
			index := 0
			// hashs := map[uint64]int{}
			for bsQueue.Len() > 0 {
				bp := bsQueue.Pop()
				if d == 0 {
					bs.effectiveProcedures[d][index] = bp
				} else {
					// g := bs.effectiveStates[d-1][bp.prevState].Copy()
					// hash := uint64(bp.prevState)
					// num, exist := hashs[hash]
					// if !exist {
					// 	hashs[hash] = 1
					// } else if num <= 6 {
					// 	hashs[hash]++
					// } else {
					// 	continue
					// }
					bs.effectiveProcedures[d][index] = bp
					// hashs[hash] = struct{}{}
				}
				index++
			}
			// EffectiveStatesにいれた候補数を格納
			bs.numEffectiveStates[d] = index
			// fmt.Fprintln(os.Stderr, bs.depth, d, index, bs.effectiveProcedures[d][0].beamValue, bs.effectiveProcedures[d][index-1].beamValue)
			bsQueue.Init()

			if bs.numEffectiveStates[d] == 0 || d == gameConst.NumBox*100/4 {
				// あんまり好ましい状態ではないが、探索の完了している状態 (bs.depthの設定を見直したほうがいいかも?)
				fmt.Fprintln(os.Stderr, "WARN: bs.Depth changed", bs.depth, "->", d)
				bs.depth = d
			}
			if ADAPTIVE {
				if d > 0 && time.Since(bs.start).Microseconds() > TIME_LIMIT_STRICT {
					fmt.Fprintln(os.Stderr, "WARN: bs.Depth changed", bs.depth, "->", d)
					bs.depth = d
				}
			}

			if d == bs.depth {
				bs.calculated = true
				if !SUCCESSIVE {
					fmt.Fprintln(os.Stderr, -bs.effectiveProcedures[d][0].beamValue)
				}
				for i := 0; i < bs.numEffectiveStates[d]; i++ {
					cg := gameConst.bsPool.Get().(*BeamSearchState)
					cg.Load(bs.effectiveStates[d-1][bs.effectiveProcedures[d][i].prevState])
					cg.ApplyAction(bs.effectiveProcedures[d][i].prevAction, estimatedSizes, gameConst, bs.minNeededSpace, border)
					bs.effectiveStates[d][i] = cg
				}

				// fmt.Fprintln(os.Stderr, bs.effectiveStates[d][0].lineWidth)
				break
			}

			createNewBP := false
			bp := NewBeamProcedure()
			for i := 0; i < bs.numEffectiveStates[d]; i++ {
				if d != bs.depth-TURNING_POINT && !bsQueue.Promising(bs.effectiveProcedures[d][i].beamValue) {
					// もう有力なものはない
					continue
					// bs.numEffectiveStates[d] = i
					// break
				}
				if d == 0 {
					cg := gameConst.bsPool.Get().(*BeamSearchState)
					cg.Load(gameState)
					bs.effectiveStates[d][i] = cg
				} else {
					cg := gameConst.bsPool.Get().(*BeamSearchState)
					cg.Load(bs.effectiveStates[d-1][bs.effectiveProcedures[d][i].prevState])
					cg.ApplyAction(bs.effectiveProcedures[d][i].prevAction, estimatedSizes, gameConst, bs.minNeededSpace, border)
					bs.effectiveStates[d][i] = cg
				}

				// 第i番目の有力候補を見る
				nowState := bs.effectiveStates[d][i]

				// 有効手についてloop
				maxRotate := 2
				if absInt(estimatedSizes[nowState.progress][0]-estimatedSizes[nowState.progress][1]) < gameConst.EstimatedSigma {
					maxRotate = 1
				}
				for prevId := 0; prevId <= nowState.progress; prevId++ {
					if prevId < nowState.progress && nowState.rightEnds[prevId] < 0 {
						continue
					}
					for rotate := 0; rotate < maxRotate; rotate++ {
						action := (prevId << 1) ^ rotate
						// evaluateが実際に後の盤面を計算しなくても差分でできる場合は先に打ち切る判断が可能
						// value := nowState.beamValue
						// value += bs.EvaluateDiff(nowState, action, gameConst)
						// if !bsQueue.Promising(value) {
						// 	// もう上位に入る可能性がないのでcontinue
						// 	continue
						// }

						r := 0.0
						if nowState.progress < gameConst.NumBox-TURNING_POINT {
							r = gameConst.Random.Float64()
							r = -r
							r *= 45000 * 100000
						} else {
							r = -gameConst.Random.Float64() * float64(gameConst.EstimatedSigma)
						}
						if !bsQueue.Promising(bs.effectiveProcedures[d][i].beamValue + r) {
							continue
						}
						if !nowState.IsValid(action, estimatedSizes, gameConst, border, bs.minNeededSpace) {
							continue
						}
						// 行動を試す
						if createNewBP {
							bp = NewBeamProcedure()
							createNewBP = false
						} else {
							// メモリの無駄遣いはやめてエコにしようね
							// bp.Load()
						}

						bp.prevState = i
						bp.prevAction = action

						// 状態を評価
						if nowState.progress >= gameConst.NumBox-TURNING_POINT {
							bp.beamValue = bs.EasyEvaluateBefore(nowState, estimatedSizes, action, gameConst, border, estimatedMinScore)
						} else {
							bp.beamValue = bs.EvaluateBefore(nowState, estimatedSizes, action, gameConst, border, estimatedMinScore)
						}
						bp.randValue = r
						bp.beamValue += bp.randValue
						// Pushしたものがqueueに入らなかったら次はCopyする必要がない
						createNewBP = bsQueue.Push(bp)
					}
				}
			}
			// ひとつ前の状態はpoolに返却
			if d > 0 {
				for i := 0; i < bs.numEffectiveStates[d-1]; i++ {
					if bs.effectiveStates[d-1][i] != nil {
						gameConst.bsPool.Put(bs.effectiveStates[d-1][i])
					}
				}
			}
		}
	}
}

func (bs *BeamSearch) GetAction() (int, *BeamSearchState) {
	// 復元(はじめにどの手を採用すればいいのか)
	index := bs.bestIndex
	for d := bs.depth; d > bs.nowDepth+1; d-- {
		index = bs.effectiveProcedures[d][index].prevState
	}
	a := bs.effectiveProcedures[bs.nowDepth+1][index].prevAction
	bs.nowDepth++ // 次回再び読み込んだ場合はすでに計算済み 次の手を返すようnowDepthを更新
	return a, bs.effectiveStates[bs.depth][bs.bestIndex]
}

func (bs *BeamSearch) ReleaseBeamStates(gameConst *GameConst) {
	for i := 0; i < bs.numEffectiveStates[bs.depth]; i++ {
		if bs.effectiveStates[bs.depth][i] != nil {

			gameConst.bsPool.Put(bs.effectiveStates[bs.depth][i])
		}
	}
}

// 連続して追加->連続してdeleteminの場合のみ使えるPriorityQueue
// 長さ（BeamSearchの幅）指定あり
type BeamSearchQueue struct {
	items     []*BeamProcedure
	popIndex  int
	pushIndex int
	width     int
	sorted    bool

	boarder float64
}

// 新しいQueueを生成する関数
func NewBeamSearchQueue(width int) *BeamSearchQueue {
	q := BeamSearchQueue{}
	q.items = make([]*BeamProcedure, 2*width)
	q.popIndex = 0
	q.pushIndex = 0
	q.boarder = -INF
	q.width = width
	return &q
}

// Queueを初期化する関数(幅の再設定は要書き換え)
func (q *BeamSearchQueue) Init() {
	q.popIndex = 0
	q.pushIndex = 0
	q.width = len(q.items) / 2
	q.boarder = -INF
	q.sorted = false
}

// Queueから最もbeamValue大きいものをとる操作
func (q *BeamSearchQueue) Pop() *BeamProcedure {
	ret := q.items[q.popIndex]
	q.popIndex++
	return ret
}

// StateをQueueに入れる操作
func (q *BeamSearchQueue) Push(item *BeamProcedure) bool {
	pushed := false
	if q.pushIndex >= len(q.items) {
		q.Sort()
	}
	if q.boarder > item.beamValue {
		// この段階で棄却
		return pushed
	}
	pushed = true
	q.sorted = false
	q.items[q.pushIndex] = item
	q.pushIndex++
	return pushed
}

// valueの大きい順に並べ替える
func (q *BeamSearchQueue) Sort() {
	sort.Slice(q.items[:q.pushIndex], func(i, j int) bool {
		return q.items[i].beamValue > q.items[j].beamValue
	})
	if q.pushIndex > q.width {
		// 上位width分以外は捨てる
		q.pushIndex = q.width
		q.boarder = q.items[q.width-1].beamValue
	}
	q.sorted = true
}

func (q *BeamSearchQueue) Promising(v float64) bool {
	return q.boarder <= v
}

func (q *BeamSearchQueue) Len() int {
	// 正しくはこれだが、安全性を無視するなら二つ目のほうが高速
	m := minInt(q.width, q.pushIndex)
	l := maxInt(m-q.popIndex, 0)
	// l := q.pushIndex - q.popIndex
	return l
}

// valueに対して昇順に並ぶ
type DoublyLinkedList struct {
	prev   []int // ひとつ前のindex
	next   []int // ひとつ後ろのindex
	values []int // 各indexでの値(昇順対象)
	height []int // 各indexでの高さ
	first  int   // 先頭のindex
	last   int   // 末尾のindex
	cap    int
	len    int
}

func NewDoublyLinkedList(cap int) *DoublyLinkedList {
	dll := DoublyLinkedList{}
	dll.prev = make([]int, cap)
	dll.next = make([]int, cap)
	dll.values = make([]int, cap)
	dll.height = make([]int, cap)
	dll.first = -1
	dll.last = -1
	dll.cap = cap
	dll.len = 0
	return &dll
}

func (dll *DoublyLinkedList) Copy() *DoublyLinkedList {
	newdll := NewDoublyLinkedList(dll.cap)
	newdll.Load(dll)
	return newdll
}
func (dll *DoublyLinkedList) Load(dll2 *DoublyLinkedList) {
	for i := 0; i < dll2.len; i++ {
		dll.prev[i] = dll2.prev[i]
	}
	for i := 0; i < dll2.len; i++ {
		dll.next[i] = dll2.next[i]
	}
	for i := 0; i < dll2.len; i++ {
		dll.values[i] = dll2.values[i]
	}
	for i := 0; i < dll2.len; i++ {
		dll.height[i] = dll2.height[i]
	}
	dll.first = dll2.first
	dll.last = dll2.last
	dll.cap = dll2.cap
	dll.len = dll2.len
}

func (l *DoublyLinkedList) Init() {
	for i := range l.prev {
		l.prev[i] = -1
	}
	for i := range l.next {
		l.next[i] = -1
	}
	l.first = -1
	l.last = -1
	// l.SetValue(0, 0)
	// l.SetHeight(0, 0)
	// l.Insert(-1, 0)
	// l.SetValue(0, 1)
	// l.SetHeight(0, 0)
	// l.Insert(0, 1)
}

// 対象範囲のheightを更新する関数　総変化量を返す
func (l *DoublyLinkedList) UpdateHeights(height int, rMin int, rMax int) int {
	sumDiff := 0
	index := l.first
	prevIndex := -1
	prevHeight := 0
	for {
		if index == -1 {
			index = prevIndex
			break
		}
		// updateでは等号含む
		if l.values[index] >= rMin {
			index = prevIndex
			break
		}
		prevIndex = index
		prevHeight = l.height[index]
		index = l.next[index]
	}
	newIndex := l.len
	l.SetValue(newIndex, rMin)
	l.SetHeight(newIndex, height)
	l.Insert(index, newIndex)
	prevValue := rMin
	d := 0
	for {
		index = l.next[newIndex]
		if index == -1 {
			break
		}
		// updateでは等号を含まない
		if l.values[index] > rMax {
			// ここは影響範囲外
			break
		}
		sumDiff -= (l.values[index] - prevValue) * prevHeight
		d += (l.values[index] - prevValue)
		prevHeight = l.height[index]
		prevValue = l.values[index]
		newIndex, _ = l.Delete(index)
	}
	sumDiff -= (rMax - prevValue) * prevHeight
	d += rMax - prevValue
	sumDiff += (rMax - rMin) * height

	newIndex2 := l.len
	l.SetValue(newIndex2, rMax)
	l.SetHeight(newIndex2, prevHeight)
	l.Insert(newIndex, newIndex2)
	return sumDiff
}

// 対象範囲のheight(最大値)を取得する関数
func (l *DoublyLinkedList) GetHeight(rMin int, rMax int) int {
	maxHeight := 0
	index := l.first
	prevIndex := -1
	prevHeight := 0
	for index != -1 {
		if l.values[index] > rMin {
			// このひとつ前からが影響範囲
			index = prevIndex
			break
		}
		prevIndex = index
		prevHeight = l.height[index]
		index = l.next[index]
	}
	prevValue := rMin
	sumDiff := 0
	for index != -1 {
		if l.values[index] >= rMax {
			// ここは影響範囲外
			break
		}
		maxHeight = maxInt(maxHeight, l.height[index])
		sumDiff -= (l.values[index] - prevValue) * prevHeight
		prevIndex = index
		prevValue = l.values[index]
		prevHeight = l.height[index]
		index = l.next[index]
	}
	sumDiff -= (rMax - prevValue) * prevHeight
	sumDiff += (rMax - rMin) * maxHeight
	return maxHeight
}

// 対象範囲の損失面積を取得する関数
func (l *DoublyLinkedList) GetLoss(height int, rMin int, rMax int) int {
	index := l.first
	prevIndex := -1
	prevHeight := 0
	for index != -1 {
		if l.values[index] > rMin {
			// このひとつ前からが影響範囲
			index = prevIndex
			break
		}
		prevIndex = index
		prevHeight = l.height[index]
		index = l.next[index]
	}
	prevValue := rMin
	sumDiff := 0
	for index != -1 {
		if l.values[index] >= rMax {
			// ここは影響範囲外
			break
		}
		sumDiff -= (l.values[index] - prevValue) * prevHeight
		prevIndex = index
		prevValue = l.values[index]
		prevHeight = l.height[index]
		index = l.next[index]
	}
	sumDiff -= (rMax - prevValue) * prevHeight
	sumDiff += (rMax - rMin) * height
	return sumDiff
}
func (l *DoublyLinkedList) GetAdditionalLoss(height int, rMin int, rMax int, additionalSpace int, update bool, border int) int {
	additionalSpace += 70000
	sumDiff := 0
	index := l.first
	prevIndex := -1
	prevHeight := INF
	down := false
	ppi := -1
	ni := index
	for index != -1 {
		if l.values[index] >= rMin {
			// このひとつ前からが影響範囲
			ni = prevIndex
			if l.values[index] == rMin {
				ni = index
			}
			index = prevIndex
			break
		}
		down = prevHeight > l.height[index]
		ppi = prevIndex
		prevIndex = index
		prevHeight = l.height[index]
		index = l.next[index]
	}
	if down && index != -1 {
		if ppi != -1 {
			if l.values[index] > rMin-additionalSpace && l.height[index] < height {
				h2 := minInt(l.height[ppi], height)

				diff := l.GetLoss(h2, l.values[index], rMin)
				sumDiff += diff
				if update {
					l.UpdateHeights(h2, l.values[index], rMin)
				}
				// if diff < 0 {
				// 	panic("")
				// }
				// fmt.Fprintln(os.Stderr, sumDiff)
			}
		}
	}
	if ni != index {
		index = ni
		prevHeight = l.height[index]
	}
	prevValue := rMin
	_ = prevValue
	// additionalSpace -= 40000
	for index != -1 {
		if l.values[index] > rMax {
			// ここは影響範囲外
			break
		}
		// sumDiff -= (l.values[index] - prevValue) * prevHeight
		prevIndex = index
		prevValue = l.values[index]
		prevHeight = l.height[index]
		index = l.next[index]
	}
	if index != -1 {
		if prevHeight < l.height[index] {
			// 谷
			if l.values[index] <= rMax+additionalSpace {
				h2 := minInt(l.height[index], height)
				diff := l.GetLoss(h2, rMax, l.values[index])
				sumDiff += diff
				// if diff < 0 {
				// 	fmt.Fprintln(os.Stderr, diff, rMax, l.values[index])
				// 	// panic("")
				// }
				if update {
					l.UpdateHeights(h2, rMax, l.values[index])
				}
			}
		}
	}
	return sumDiff
}

func (l *DoublyLinkedList) SetValue(index int, value int) {
	l.values[index] = value
}
func (l *DoublyLinkedList) SetHeight(index int, height int) {
	l.height[index] = height
}

func (l *DoublyLinkedList) Insert(i, j int) {
	// iの後ろにjをつなげる（割り込ませる）操作
	k := l.first
	if i != -1 {
		k = l.next[i]
	}
	l.next[j] = k
	if i != -1 {
		l.next[i] = j
	}
	if k != -1 {
		l.prev[k] = j
	}
	if i == -1 {
		l.first = j
	}
	if i == l.last {
		l.last = j
	}
	l.prev[j] = i
	l.len++
}

// 削除し、前後のindexを返す
func (l *DoublyLinkedList) Delete(j int) (int, int) {
	// jを消去する操作
	i := l.prev[j]
	k := l.next[j]
	if i != -1 {
		l.next[i] = k
	}
	if j == l.first {
		l.first = k
	}
	if k != -1 {
		l.prev[k] = i
	}
	if j == l.last {
		l.last = i
	}
	l.prev[j] = -1
	l.next[j] = -1
	l.len--

	if j != l.len {
		// 前に詰める
		l.prev[j] = l.prev[l.len]
		l.next[j] = l.next[l.len]
		l.values[j] = l.values[l.len]
		l.height[j] = l.height[l.len]
		if l.prev[j] != -1 {
			l.next[l.prev[j]] = j
		} else {
			l.first = j
		}
		if l.next[j] != -1 {
			l.prev[l.next[j]] = j
		} else {
			l.last = j
		}
	}
	if i == l.len {
		i = j
	}
	if k == l.len {
		k = j
	}
	return i, k
}

func (l *DoublyLinkedList) DebugPrint() {
	index := l.first
	for index != -1 {
		fmt.Println("# borderline", l.values[index], l.height[index])
		index = l.next[index]
	}
	// fmt.Println("# borderline", l.values)
	// fmt.Println("# borderline", l.height)
	// fmt.Println("# borderline", l.prev)
	// fmt.Println("# borderline", l.next)
}
func (l *DoublyLinkedList) ToHash() uint64 {
	hash := uint64(0)
	prevHeight := -1
	index := l.first
	count := 0
	for index != -1 {
		if l.height[index] > prevHeight {
			hash ^= HASH_BORDERLINE[index]
		} else {
			hash ^= HASH_BORDERLINE_MINUS[index]
		}
		prevHeight = l.height[index]
		// height := l.height[index]
		// hash += uint64(height * height)
		index = l.next[index]
		count++
	}
	return hash
}

type SimulatedAnnealingPlan struct {
	size       [][2]int
	difference []int
}

func NewSimulatedAnnealingPlan(numBox int, lenHistory int) *SimulatedAnnealingPlan {
	plan := &SimulatedAnnealingPlan{}
	plan.size = make([][2]int, numBox)
	plan.difference = make([]int, lenHistory)
	return plan
}

func (plan *SimulatedAnnealingPlan) Copy() *SimulatedAnnealingPlan {
	newP := NewSimulatedAnnealingPlan(len(plan.size), len(plan.difference))
	newP.Load(plan)
	return newP
}
func (plan *SimulatedAnnealingPlan) Load(plan2 *SimulatedAnnealingPlan) {
	for i := range plan2.size {
		for d := 0; d < 2; d++ {
			plan.size[i][d] = plan2.size[i][d]
		}
	}
	for i := range plan2.difference {
		plan.difference[i] = plan2.difference[i]
	}
}

func (plan *SimulatedAnnealingPlan) ApplyAction(gameConst *GameConst, history [][]int, target int, amount int) {
	dir := target & 1
	boxId := target >> 1
	plan.size[boxId][dir] += amount
	for i := range history {
		if history[i][boxId] == dir {
			plan.difference[i] += amount
		}
	}
}

func (plan *SimulatedAnnealingPlan) RevertAction(gameConst *GameConst, history [][]int, target int, amount int) {
	plan.ApplyAction(gameConst, history, target, -amount)
}

func (plan *SimulatedAnnealingPlan) Evaluate(gameConst *GameConst) float64 {
	// sumSqDiff := 0
	value := 0.0
	for i := range plan.difference {
		// sumSqDiff += plan.difference[i] * plan.difference[i]
		d := absInt(plan.difference[i]) * 100 / gameConst.Sigma
		logd := 0.0
		if d > 500 {
			logd -= float64(d - 500)
			d = 500
		}
		value += logd + logds[d]
		// if i >= len(plan.difference)-gameConst.NumBox {
		// 	value += logds[d]
		// }
	}
	return value
}

func (plan *SimulatedAnnealingPlan) EvaluateDiff(gameConst *GameConst, target int, amount int) float64 {
	return 0.0
}

type SimulatedAnnealing struct {
	startTime             time.Time
	duration              int     // 焼きなまし実行時間　microsecond
	startTemp             float64 // 開始時の温度
	endTemp               float64 // 終了時の温度
	updateTempIntervalExp int     // 温度の更新間隔 1<<updateTempIntervalexp が間隔になる

	// 状態
	sumIter        int                     // loopの回った回数
	valid          int                     // validな解を得た回数
	accepted       int                     // 採用された回数
	temp           float64                 // 現在の温度
	reciprocaltemp float64                 // 温度の逆数
	best           *SimulatedAnnealingPlan // これまでの最善解
	bestScore      float64                 // 最善解のスコア
	now            *SimulatedAnnealingPlan // 現在の遷移元
	nowScore       float64                 // 現在の遷移元のスコア
}

func NewSA() *SimulatedAnnealing {
	sa := SimulatedAnnealing{}
	return &sa
}

func (sa *SimulatedAnnealing) getAcceptRate(score float64, bestScore float64) float64 {
	diff := score - bestScore
	p := sa.getAcceptRateFromDiff(diff, bestScore)
	return p
}
func (sa *SimulatedAnnealing) getAcceptRateFromDiff(diff float64, bestScore float64) float64 {
	p := math.Exp(diff * sa.reciprocaltemp)
	return p
}

func (sa *SimulatedAnnealing) isBest(score float64, bestScore float64) bool {
	return score > bestScore

}

func (sa *SimulatedAnnealing) Init(duration int, startTemp float64, endTemp float64, best *SimulatedAnnealingPlan, bestScore float64) {
	sa.startTime = time.Now()
	sa.startTemp = startTemp
	sa.endTemp = endTemp

	sa.duration = duration
	sa.best = best
	sa.bestScore = bestScore
	sa.now = best.Copy()
	sa.nowScore = bestScore

}

func (sa *SimulatedAnnealing) Run(gameConst *GameConst, history [][]int) {
	prop := 0.0
	sa.updateTempIntervalExp = 9
	for {
		if (sa.sumIter & (1<<sa.updateTempIntervalExp - 1)) == 0 {
			prop = float64(time.Since(sa.startTime).Microseconds()) / float64(sa.duration)
			if prop >= 1 {
				// time up
				break
			}
			if DEBUG {
				// fmt.Fprintf(os.Stderr, "prop: %.2f, iter: %d, bestScore: %f (now: %f)\n", prop, sa.sumIter, sa.bestScore, sa.nowScore)
			}
			// 指数ver.
			sa.temp = math.Pow(sa.startTemp, 1-prop) * math.Pow(sa.endTemp, prop)
			// 線形ver.
			// sa.temp = sa.startTemp*(1-prop) + sa.endTemp*prop
			sa.reciprocaltemp = 1 / sa.temp

			// 最善解に戻す
			// sa.now = sa.best.Copy()
			// sa.nowScore = sa.bestScore
		}
		sa.sumIter++

		// ここで何かnowを変更する
		target := gameConst.Random.Intn(gameConst.NumBox << 1)
		// 差分が先に計算できる場合はこちらを採用
		// scoreDiff := sa.now.EvaluateDiff(gameConst, action)
		// p := sa.getAcceptRateFromDiff(scoreDiff, sa.bestScore)
		// r := gameConst.Random.Float64()
		// if p < r {
		// 	continue
		// }
		// score := sa.nowScore + scoreDiff
		amount := int(float64(gameConst.Sigma) / (100 + gameConst.Random.Float64()*50) * float64(gameConst.Random.Intn(2)*2-1))
		boxId := target >> 1
		rotate := target & 1
		newSize := sa.now.size[boxId][rotate] + amount
		if newSize > 100000-gameConst.EstimatedSigma/2 || newSize < 10000+gameConst.EstimatedSigma/2 {
			continue
		}
		sa.now.ApplyAction(gameConst, history, target, amount)
		sa.valid++
		score := sa.now.Evaluate(gameConst)
		p := sa.getAcceptRate(score, sa.nowScore)
		// p := sa.getAcceptRate(score, sa.bestScore)
		r := gameConst.Random.Float64()
		if p >= r {
			// 採用
			sa.accepted++
			sa.nowScore = score
			if sa.isBest(score, sa.bestScore) {
				// 最善解を変更
				sa.best = sa.now.Copy()
				sa.bestScore = sa.nowScore
			}
		} else {
			// rollback（nowを戻す）
			sa.now.RevertAction(gameConst, history, target, amount)
		}
	}
	// fmt.Fprintln(os.Stderr, "--- SA Summary ---")
	// fmt.Fprintln(os.Stderr, "sumIter: ", sa.sumIter)
	// fmt.Fprintln(os.Stderr, "valid: ", sa.valid)
	// fmt.Fprintln(os.Stderr, "accepted: ", sa.accepted)
	// fmt.Fprintln(os.Stderr, "bestScore: ", sa.bestScore)
}
