#pragma GCC optimize "-O3,omit-frame-pointer,inline,unroll-all-loops,fast-math"
 #pragma GCC target "tune=native"
 #include <bits/stdc++.h>
 #include <sys/time.h>
const double TL_SIMPLEX = 1.8;
const double TL = 1.95;
double get_elapsed();
// Qi Huangfu's simplex solver (https://github.com/jajhall/hsol/tree/master)
// (MIT License)
// #include <vector>
using namespace std;
class HVector {
public:
    // Main part
    void setup(int size_);
    void clear();
    int size;
    int count; // count of non zeros
    vector<int> index; // index of non zeros
    vector<double> array; // array
    int pseudoTick;
    double fakeTick;
    // For update
    vector<char> cwork; // char working buffer
    vector<int> iwork; // integer working buffer
    HVector *next;
    // Package
    void tight();
    void pack();
    bool packFlag; // pack flag: do pack or not
    int packCount; // pack count
    vector<int> packIndex; // pack index
    vector<double> packValue; // pack value
    // Advanced
    void copy(const HVector *from);
    double norm2();
    void saxpy(const double pivotX, const HVector *pivot);
};
typedef HVector* HVector_ptr;

// #include <vector>
using namespace std;
class HMatrix {
public:
    const int *getAstart() const {
        return &Astart[0];
    }
    const int *getAindex() const {
        return &Aindex[0];
    }
    const double *getAvalue() const {
        return &Avalue[0];
    }
    void setup(int numCol, int numRow, const int *Astart, const int *Aindex,
            const double *Avalue);
    void price_by_col(HVector& row_ap, HVector& row_ep) const;
    void price_by_row(HVector& row_ap, HVector& row_ep) const;
    void update(int columnIn, int columnOut);
    double compute_dot(HVector& vector, int iCol) const;
    void collect_aj(HVector& vector, int iCol, double multi) const;
    void compute_vecT_matB(const double *vec, const int *base, HVector *res);
    void compute_matB_vec(const double *vec, const int *base, HVector *res);
private:
    int numCol;
    int numRow;
    vector<int> Astart;
    vector<int> Aindex;
    vector<double> Avalue;
    vector<int> ARstart;
    vector<int> AR_Nend;
    vector<int> ARindex;
    vector<double> ARvalue;
};
// #include <cmath>
// #include <vector>
// #include <algorithm>
using namespace std;
enum UPDATE_METHOD {
    UPDATE_METHOD_FT = 1,
    UPDATE_METHOD_PF = 2,
    UPDATE_METHOD_MPF = 3,
    UPDATE_METHOD_APF = 4
};
class HFactor {
public:
    void copyFrom(const HFactor *from);
    void setup(int numCol, int numRow, int *Astart, int *Aindex, double *Avalue,
            int *baseIndex, int updateMethod = UPDATE_METHOD_FT);
    void change(int updateMethod);
    void build();
    void ftran(HVector& vector, double hist_dsty) const;
    void btran(HVector& vector, double hist_dsty) const;
    void update(HVector *aq, HVector *ep, int *iRow, int *hint);
    int pseudoTick;
    int BtotalX;
    int FtotalX;
    double realTick;
    double fakeTick;
private:
    /**
     * Data of the factor
     */
    // Problem size, coefficient matrix and update method
    int numRow;
    int numCol;
    const int *Astart;
    const int *Aindex;
    const double *Avalue;
    int *baseIndex;
    int updateMethod;
    // Count of elements
    // Working buffer
    int nwork;
    vector<int> iwork;
    vector<double> dwork;
    // Basis matrix
    vector<int> Bstart;
    vector<int> Bindex;
    vector<double> Bvalue;
    // Permutation
    vector<int> permute;
    // Kernel matrix
    vector<int> MCstart;
    vector<int> MCcountA;
    vector<int> MCcountN;
    vector<int> MCspace;
    vector<int> MCindex;
    vector<double> MCvalue;
    vector<double> MCminpivot;
    // Row wise kernel matrix
    vector<int> MRstart;
    vector<int> MRcount;
    vector<int> MRspace;
    vector<int> MRcountb4;
    vector<int> MRindex;
    // Kernel column buffer
    vector<int> McolumnIndex;
    vector<char> McolumnMark;
    vector<double> McolumnArray;
    // Count link list
    vector<int> clinkFirst;
    vector<int> clinkNext;
    vector<int> clinkLast;
    vector<int> rlinkFirst;
    vector<int> rlinkNext;
    vector<int> rlinkLast;
    // Factor L
    vector<int> LpivotLookup;
    vector<int> LpivotIndex;
    vector<int> Lstart;
    vector<int> Lindex;
    vector<double> Lvalue;
    vector<int> LRstart;
    vector<int> LRindex;
    vector<double> LRvalue;
    // Factor U
    vector<int> UpivotLookup;
    vector<int> UpivotIndex;
    vector<double> UpivotValue;
    int UmeritX;
    int UtotalX;
    vector<int> Ustart;
    vector<int> Ulastp;
    vector<int> Uindex;
    vector<double> Uvalue;
    vector<int> URstart;
    vector<int> URlastp;
    vector<int> URspace;
    vector<int> URindex;
    vector<double> URvalue;
    // Update buffer
    vector<double> PFpivotValue;
    vector<int> PFpivotIndex;
    vector<int> PFstart;
    vector<int> PFindex;
    vector<double> PFvalue;
    // Implementation
    void buildSimple();
    void buildKernel();
    void buildFinish();
    void ftranL(HVector& vector, double hist_dsty) const;
    void btranL(HVector& vector, double hist_dsty) const;
    void ftranU(HVector& vector, double hist_dsty) const;
    void btranU(HVector& vector, double hist_dsty) const;
    void ftranFT(HVector& vector) const;
    void btranFT(HVector& vector) const;
    void ftranPF(HVector& vector) const;
    void btranPF(HVector& vector) const;
    void ftranMPF(HVector& vector) const;
    void btranMPF(HVector& vector) const;
    void ftranAPF(HVector& vector) const;
    void btranAPF(HVector& vector) const;
    void updateCFT(HVector *aq, HVector *ep, int *iRow, int *hint);
    void updateFT(HVector *aq, HVector *ep, int iRow, int *hint);
    void updatePF(HVector *aq, HVector *ep, int iRow, int *hint);
    void updateMPF(HVector *aq, HVector *ep, int iRow, int *hint);
    void updateAPF(HVector *aq, HVector *ep, int iRow, int *hint);
    /**
     * Local in-line functions
     */
    void colInsert(const int iCol, const int iRow, const double value) {
        const int iput = MCstart[iCol] + MCcountA[iCol]++;
        MCindex[iput] = iRow;
        MCvalue[iput] = value;
    }
    void colStoreN(const int iCol, const int iRow, const double value) {
        const int iput = MCstart[iCol] + MCspace[iCol] - (++MCcountN[iCol]);
        MCindex[iput] = iRow;
        MCvalue[iput] = value;
    }
    void colFixMax(const int iCol) {
        double maxValue = 0;
        for (int k = MCstart[iCol]; k < MCstart[iCol] + MCcountA[iCol]; k++)
            maxValue = max(maxValue, fabs(MCvalue[k]));
        MCminpivot[iCol] = maxValue * 0.1;
    }
    double colDelete(const int iCol, const int iRow) {
        int idel = MCstart[iCol];
        int imov = idel + (--MCcountA[iCol]);
        while (MCindex[idel] != iRow)
            idel++;
        double pivotX = MCvalue[idel];
        MCindex[idel] = MCindex[imov];
        MCvalue[idel] = MCvalue[imov];
        return pivotX;
    }
    void rowInsert(const int iCol, const int iRow) {
        int iput = MRstart[iRow] + MRcount[iRow]++;
        MRindex[iput] = iCol;
    }
    void rowDelete(const int iCol, const int iRow) {
        int idel = MRstart[iRow];
        int imov = idel + (--MRcount[iRow]);
        while (MRindex[idel] != iCol)
            idel++;
        MRindex[idel] = MRindex[imov];
    }
    void clinkAdd(const int index, const int count) {
        const int mover = clinkFirst[count];
        clinkLast[index] = -2 - count;
        clinkNext[index] = mover;
        clinkFirst[count] = index;
        if (mover >= 0)
            clinkLast[mover] = index;
    }
    void clinkDel(const int index) {
        const int xlast = clinkLast[index];
        const int xnext = clinkNext[index];
        if (xlast >= 0)
            clinkNext[xlast] = xnext;
        else
            clinkFirst[-xlast - 2] = xnext;
        if (xnext >= 0)
            clinkLast[xnext] = xlast;
    }
    void rlinkAdd(const int index, const int count) {
        const int mover = rlinkFirst[count];
        rlinkLast[index] = -2 - count;
        rlinkNext[index] = mover;
        rlinkFirst[count] = index;
        if (mover >= 0)
            rlinkLast[mover] = index;
    }
    void rlinkDel(const int index) {
        const int xlast = rlinkLast[index];
        const int xnext = rlinkNext[index];
        if (xlast >= 0)
            rlinkNext[xlast] = xnext;
        else
            rlinkFirst[-xlast - 2] = xnext;
        if (xnext >= 0)
            rlinkLast[xnext] = xlast;
    }
};

class HRandom {
public:
    HRandom() {
        random_mw = 1985;
        random_mz = 2012;
    }
    int intRandom() {
        random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        unsigned result = (random_mz << 16) + random_mw;
        return result >> 1;
    }
    double dblRandom() {
        random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        unsigned result = (random_mz << 16) + random_mw;
        return (result + 1.0) * 2.328306435454494e-10;
    }
private:
    unsigned random_mw;
    unsigned random_mz;
};

// #include <string>
// #include <vector>
using namespace std;
const int LP_Status_Unset = -1;
const int LP_Status_Optimal = 0;
const int LP_Status_Infeasible = 1;
const int LP_Status_Unbounded = 2;
const int LP_Status_Singular = 3;
const int LP_Status_Failed = 4;
enum HSOL_INT_OPTIONS {
    INTOPT_PRINT_FLAG = 0, // 0/1 = none/do-print
    INTOPT_TRANSPOSE_FLAG, // 0/1 = none/do-transpose if possible
    INTOPT_SCALE_FLAG, // 0/1 = none/do-scale
    INTOPT_TIGHT_FLAG, // 0/1 = none/do-tight
    INTOPT_PERMUTE_FLAG, // 0/1 = none/do-permute
    INTOPT_PERTURB_FLAG, // 0/1 = none/do-perturb
    INTOPT_COUNT
};
enum HSOL_DBL_OPTIONS {
    DBLOPT_TIME_LIMIT = 0,
    DBLOPT_PRIMAL_TOL,
    DBLOPT_DUAL_TOL,
    DBLOPT_PERTURB_BASE,
    DBLOPT_PAMI_CUTOFF,
    DBLOPT_COUNT
};
enum HSOL_STR_OPTIONS {
    STROPT_PARTITION_FILE = 0, // name of row partition file
    STROPT_COUNT
};
class HModel {
public:
    HModel();
    void setupstart();
    void setupend();
  // void setup_loadMPS(const char *filename);
    void setup_transposeLP();
    void setup_scaleMatrix();
    void setup_tightenBound();
    void setup_shuffleColumn();
    void setup_allocWorking();
    int getNumRow() {
        return numRow;
    }
    int getNumCol() {
        return numCol;
    }
    int getNumTot() {
        return numTot;
    }
    const HMatrix *getMatrix() {
        return &matrix;
    }
    const HFactor *getFactor() {
        return &factor;
    }
    int *getBaseIndex() {
        return &basicIndex[0];
    }
    int *getNonbasicFlag() {
        return &nonbasicFlag[0];
    }
    int *getNonbasicMove() {
        return &nonbasicMove[0];
    }
    int *getWorkIntBreak() {
        return &intBreak[0];
    }
    double *getWorkCost() {
        return &workCost[0];
    }
    double *getWorkDual() {
        return &workDual[0];
    }
    double *getWorkShift() {
        return &workShift[0];
    }
    double *getWorkLower() {
        return &workLower[0];
    }
    double *getWorkUpper() {
        return &workUpper[0];
    }
    double *getWorkRange() {
        return &workRange[0];
    }
    double *getWorkValue() {
        return &workValue[0];
    }
    double *getBaseLower() {
        return &baseLower[0];
    }
    double *getBaseUpper() {
        return &baseUpper[0];
    }
    double *getBaseValue() {
        return &baseValue[0];
    }
    void initCost(int perturb = 0);
    void initBound(int phase = 2);
    void initValue();
    void computeFactor();
    void computeDual();
    void computeDualInfeasInDual(int *dualInfeasCount);
    void computeDualInfeasInPrimal(int *dualInfeasCount);
    void correctDual(int *freeInfeasCount);
    void computePrimal();
    void computeObject(int phase = 2);
    void shiftCost(int iCol, double amount);
    void shiftBack(int iCol);
    void flipBound(int iCol);
    void updateFactor(HVector *column, HVector *row_ep, int *iRow, int *hint);
    void updateMatrix(int columnIn, int columnOut);
    void updatePivots(int columnIn, int rowOut, int sourceOut);
    void changeUpdate(int updateMethod);
    void reportPivots(int columnIn, int columnOut, double alpha);
    void reportStatus(int status);
    void printMessage(const char *message);
    void printObject();
    void printResult();
    // Solving options
    int intOption[INTOPT_COUNT];
    double dblOption[DBLOPT_COUNT];
    string strOption[STROPT_COUNT];
    // Random generator
    HRandom random;
    // Perturbation flag
    int problemPerturbed;
    // The original models
    string modelName;
    // Solving result
    int limitUpdate;
    int countUpdate;
    int numberIteration;
    double objective;
    //private:
    // The original model
    int numCol;
    int numRow;
    int numTot;
    vector<int> Astart;
    vector<int> Aindex;
    vector<double> Avalue;
    vector<double> colCost;
    vector<double> colLower;
    vector<double> colUpper;
    vector<double> rowLower;
    vector<double> rowUpper;
    double objOffset;
    // Associated data of original model
    vector<int> workRowPart; // Row partition
    vector<int> intBreak;
    vector<double> dblXpert;
    // Working model
    int problemStatus;
    vector<int> basicIndex;
    vector<int> nonbasicFlag;
    vector<int> nonbasicMove;
    HMatrix matrix;
    HFactor factor;
    HVector buffer;
    HVector bufferLong;
    vector<double> workCost;
    vector<double> workDual;
    vector<double> workShift;
    vector<double> workLower;
    vector<double> workUpper;
    vector<double> workRange;
    vector<double> workValue;
    vector<double> baseLower;
    vector<double> baseUpper;
    vector<double> baseValue;
};
// #include <set>
// #include <vector>
using namespace std;
/**
 * This class deal with ratio test
 * and some update dual/flip tasks.
 */
class HDualRow {
public:
    void setup(HModel *model);
    void setupSlice(HModel *model, int size);
    void clear();
    void choose_makepack(const HVector *row, const int offset);
    void choose_possible();
    void choose_joinpack(const HDualRow* otherRow);
    void choose_final();
    void update_flip(HVector *bfrtColumn);
    void update_dual(double theta);
    void create_Freelist();
    void create_Freemove(HVector *row_ep);
    void delete_Freemove();
    void delete_Freelist(int iColumn);
    HModel *workModel;
    int workSize;
    const int *workRand;
    const int *workMove;
    const double *workDual;
    const double *workRange;
    set<int> freeList;
    int packCount;
    vector<int> packIndex;
    vector<double> packValue;
    double workDelta;
    double workAlpha;
    double workTheta;
    int workPivot;
    int workCount;
    vector<pair<int, double> > workData;
    vector<int> workGroup;
};
// #include <vector>
using namespace std;
/**
 * This class deal with optimality test
 * and some update primal/weight tasks
 */
class HDualRHS {
public:
    void setup(HModel *model);
    void setup_partition(const char *filename);
    void choose_normal(int *chIndex);
    void choose_multi_global(int *chIndex, int *chCount, int chLimit);
    void choose_multi_HGauto(int *chIndex, int *chCount, int chLimit);
    void choose_multi_HGpart(int *chIndex, int *chCount, int chLimit);
    void update_primal(HVector *column, double theta);
    void update_weight(HVector *column, double devex, double Kai, double *dse);
    void update_pivots(int iRow, double value);
    void update_infeasList(HVector *column);
    void create_infeasList(double columnDensity);
    void create_infeasArray();
    HModel *workModel;
    double workCutoff;
    int workCount;
    vector<char> workMark;
    vector<int> workIndex;
    vector<double> workArray;
    vector<double> workDevex;
    vector<double> workDevexFull;
    int partNum;
    int partNumRow;
    int partNumCol;
    int partNumCut;
    int partSwitch;
    vector<int> workPartition;
};

// #include <set>
// #include <string>
// #include <vector>
using namespace std;
const int HSOL_THREAD_LIMIT = 32;
const int HSOL_SLICED_LIMIT = 100;
enum HDUAL_VARIANT {
    HDUAL_VARIANT_PLAIN = 0, HDUAL_VARIANT_TASKS, HDUAL_VARIANT_MULTI,
};
class HDual {
public:
    void solve(HModel *model, int variant = 0, int num_threads = 1);
public:
    void init(int num_threads);
    void init_slice(int init_sliced_num);
    void solve_phase1();
    void solve_phase2();
    void rebuild();
    void cleanup();
    void iterate();
    void iterate_tasks();
    void iterate_multi();
    void chooseRow();
    void chooseColumn(HVector *row_ep);
    void chooseColumn_slice(HVector *row_ep);
    void updateFtranBFRT();
    void updateFtran();
    void updateFtranDSE(HVector *dseVector);
    void updateVerify();
    void updateDual();
    void updatePrimal(HVector *dseVector);
    void updatePivots();
    void major_chooseRow();
    void major_chooseRowBtran();
    void minor_chooseRow();
    void minor_update();
    void minor_updateDual();
    void minor_updatePrimal();
    void minor_updatePivots();
    void minor_updateRows();
    void major_update();
    void major_updateFtranPrepare();
    void major_updateFtranParallel();
    void major_updateFtranFinal();
    void major_updatePrimal();
    void major_updateFactor();
    void major_rollback();
    // Variant choice
    int dual_variant;
    // Model
    HModel *model;
    double Tp; // Tolerance for primal
    double Td; // Tolerance for dual
    int numCol;
    int numRow;
    int numTot;
    const HMatrix *matrix;
    const HFactor *factor;
    const int *jMove;
    const double *workRange;
    const double *baseLower;
    const double *baseUpper;
    double *baseValue;
    double *workDual;
    int solvePhase;
    int invertHint;
    HVector row_ep;
    HVector row_ap;
    HVector column;
    HVector columnBFRT;
    HVector columnDSE;
    double row_epDensity;
    double rowdseDensity;
    double columnDensity;
    HDualRow dualRow;
    // Solving related buffers
    int dualInfeasCount;
    HDualRHS dualRHS;
    // Simplex pivotal information
    int rowOut;
    int columnOut;
    int sourceOut; // -1 from small to lower, +1 to upper
    int columnIn;
    double deltaPrimal;
    double thetaDual;
    double thetaPrimal;
    double alpha;
    double alphaRow;
    // Partitioned coefficient matrix
    int slice_num;
    int slice_PRICE;
    int slice_start[HSOL_SLICED_LIMIT + 1];
    HMatrix slice_matrix[HSOL_SLICED_LIMIT];
    HVector slice_row_ap[HSOL_SLICED_LIMIT];
    HDualRow slice_dualRow[HSOL_SLICED_LIMIT];
    // Multiple price data
    struct MChoice {
        int rowOut;
        double baseValue;
        double baseLower;
        double baseUpper;
        double infeasValue;
        double infeasDevex;
        double infeasLimit;
        HVector row_ep;
        HVector column;
        HVector columnBFRT;
    };
    struct MFinish {
        int moveIn;
        double shiftOut;
        vector<int> flipList;
        int rowOut;
        int columnOut;
        int columnIn;
        double alphaRow;
        double thetaPrimal;
        double basicBound;
        double basicValue;
        double devex;
        HVector_ptr row_ep;
        HVector_ptr column;
        HVector_ptr columnBFRT;
    };
    int multi_num;
    int multi_iChoice;
    int multi_nFinish;
    int multi_iteration;
    int multi_chooseAgain;
    MChoice multi_choice[HSOL_THREAD_LIMIT];
    MFinish multi_finish[HSOL_THREAD_LIMIT];
    double total_fake;
    double total_INVERT_TICK;
    double total_FT_inc_TICK;
};
const double HSOL_CONST_INF = 1e200;
const double HSOL_CONST_TINY = 1e-14;
const double HSOL_CONST_ZERO = 1e-50;
class HPrimal {
public:
    void solvePhase2(HModel *ptr_model);
private:
    void primalRebuild();
    void primalChooseColumn();
    void primalChooseRow();
    void primalUpdate();
    // Model pointer
    HModel *model;
    int numCol;
    int numRow;
    int numTot;
    // Pivot related
    int limitUpdate;
    int countUpdate;
    int invertHint;
    int columnIn;
    int rowOut;
    // Solve buffer
    HVector row_ep;
    HVector row_ap;
    HVector column;
    double row_epDensity;
    double columnDensity;
};

// #include <iostream>
// #include <cstdio>
// #include <cmath>
// #include <set>
// #include <stdexcept>
using namespace std;
void HDual::solve(HModel *ptr_model, int variant, int num_threads) {
    dual_variant = variant;
    model = ptr_model;
    if (model->getNumRow() == 0)
        return;
    // Initialise working environment
    init(num_threads);
    model->initCost(1);
    model->computeFactor();
    model->computeDual();
    model->computeDualInfeasInDual(&dualInfeasCount);
    solvePhase = dualInfeasCount > 0 ? 1 : 2;
    // Find largest dual and adjust the dual tolerance accordingly
    double largeDual = 0;
    for (int i = 0; i < numTot; i++) {
        if (model->getNonbasicFlag()[i]) {
            double myDual = fabs(workDual[i] * jMove[i]);
            if (largeDual < myDual)
                largeDual = myDual;
        }
    }
    // The major solving loop
    while (solvePhase) {
        switch (solvePhase) {
        case 1:
            solve_phase1();
            break;
        case 2:
            solve_phase2();
            break;
        case 4:
            break;
        default:
            solvePhase = 0;
            break;
        }
        // Jump for primal
        if (solvePhase == 4)
            break;
    }
    // Use primal to clean up
    if (solvePhase == 4) {
        HPrimal hPrimal;
        hPrimal.solvePhase2(model);
    }
}
void HDual::init(int num_threads) {
    // Copy size, matrix and factor
    numCol = model->getNumCol();
    numRow = model->getNumRow();
    numTot = model->getNumTot();
    matrix = model->getMatrix();
    factor = model->getFactor();
    // Copy pointers
    jMove = model->getNonbasicMove();
    workDual = model->getWorkDual();
    workRange = model->getWorkRange();
    baseLower = model->getBaseLower();
    baseUpper = model->getBaseUpper();
    baseValue = model->getBaseValue();
    // Copy tolerances
    Tp = model->dblOption[DBLOPT_PRIMAL_TOL];
    Td = model->dblOption[DBLOPT_DUAL_TOL];
    // Setup local vectors
    columnDSE.setup(numRow);
    columnBFRT.setup(numRow);
    column.setup(numRow);
    row_ep.setup(numRow);
    row_ap.setup(numCol);
    columnDensity = 0;
    row_epDensity = 0;
    rowdseDensity = 0;
    // Setup other buffers
    dualRow.setup(model);
    dualRHS.setup(model);
    // Initialize for tasks
    if (dual_variant == HDUAL_VARIANT_TASKS) {
        init_slice(num_threads - 2);
    }
    // Initialize for multi
    if (dual_variant == HDUAL_VARIANT_MULTI) {
        multi_num = num_threads;
        if (multi_num < 1)
            multi_num = 1;
        if (multi_num > HSOL_THREAD_LIMIT)
            multi_num = HSOL_THREAD_LIMIT;
        for (int i = 0; i < multi_num; i++) {
            multi_choice[i].row_ep.setup(numRow);
            multi_choice[i].column.setup(numRow);
            multi_choice[i].columnBFRT.setup(numRow);
        }
        init_slice(multi_num - 1);
    }
    multi_iteration = 0;
    string partitionFile = model->strOption[STROPT_PARTITION_FILE];
    if (partitionFile.size()) {
        dualRHS.setup_partition(partitionFile.c_str());
    }
}
void HDual::init_slice(int init_sliced_num) {
    // Number of slices
    slice_num = init_sliced_num;
    if (slice_num < 1)
        slice_num = 1;
    if (slice_num > HSOL_SLICED_LIMIT)
        slice_num = HSOL_SLICED_LIMIT;
    // Alias to the matrix
    const int *Astart = matrix->getAstart();
    const int *Aindex = matrix->getAindex();
    const double *Avalue = matrix->getAvalue();
    const int AcountX = Astart[numCol];
    // Figure out partition weight
    double sliced_countX = AcountX / slice_num;
    slice_start[0] = 0;
    for (int i = 0; i < slice_num - 1; i++) {
        int endColumn = slice_start[i] + 1; // At least one column
        int endX = Astart[endColumn];
        int stopX = (i + 1) * sliced_countX;
        while (endX < stopX) {
            endX = Astart[++endColumn];
        }
        slice_start[i + 1] = endColumn;
        if (endColumn >= numCol) {
            slice_num = i; // SHRINK
            break;
        }
    }
    slice_start[slice_num] = numCol;
    // Partition the matrix, row_ap and related packet
    vector<int> sliced_Astart;
    for (int i = 0; i < slice_num; i++) {
        // The matrix
        int mystart = slice_start[i];
        int mycount = slice_start[i + 1] - mystart;
        int mystartX = Astart[mystart];
        sliced_Astart.resize(mycount + 1);
        for (int k = 0; k <= mycount; k++)
            sliced_Astart[k] = Astart[k + mystart] - mystartX;
        slice_matrix[i].setup(mycount, numRow, &sliced_Astart[0],
                Aindex + mystartX, Avalue + mystartX);
        // The row_ap and its packages
        slice_row_ap[i].setup(mycount);
        slice_dualRow[i].setupSlice(model, mycount);
    }
}
void HDual::solve_phase1() {
    model->printMessage("dual-phase-1-start");
    // Switch to dual phase 1 bounds
    model->initBound(1);
    model->initValue();
    // Main solving structure
    for (;;) {
      if(get_elapsed() > TL_SIMPLEX) throw 0;
        rebuild();
        for (;;) {
            switch (dual_variant) {
            default:
            case HDUAL_VARIANT_PLAIN:
                iterate();
                break;
            case HDUAL_VARIANT_TASKS:
                iterate_tasks();
                break;
            case HDUAL_VARIANT_MULTI:
                iterate_multi();
                break;
            }
            if (invertHint)
                break;
        }
        if (model->countUpdate == 0)
            break;
    }
    if (rowOut == -1) {
        model->printMessage("dual-phase-1-optimal");
        // Go to phase 2
        if (model->objective == 0) {
            solvePhase = 2;
        } else {
            // We still have dual infeasible
            if (model->problemPerturbed) {
                // Clean up perturbation and go on
                cleanup();
                if (dualInfeasCount == 0)
                    solvePhase = 2;
            } else {
                // Report dual infeasible
                solvePhase = -1;
                model->printMessage("dual-infeasible");
                model->reportStatus(LP_Status_Unbounded);
            }
        }
    } else if (columnIn == -1) {
        // We got dual phase 1 unbounded - strange
        model->printMessage("dual-phase-1-unbounded");
        if (model->problemPerturbed) {
            // Clean up perturbation and go on
            cleanup();
            if (dualInfeasCount == 0)
                solvePhase = 2;
        } else {
            // Report strange issues
            solvePhase = -1;
            model->printMessage("dual-phase-1-not-solved");
            model->reportStatus(LP_Status_Failed);
        }
    }
    if (solvePhase == 2) {
        model->initBound();
        model->initValue();
    }
}
void HDual::solve_phase2() {
    model->printMessage("dual-phase-2-start");
    // Collect free variables
    dualRow.create_Freelist();
    // Main solving structure
    for (;;) {
      if(get_elapsed() > TL_SIMPLEX) throw 0;
        rebuild();
        if (dualInfeasCount > 0)
            break;
        for (;;) {
            switch (dual_variant) {
            default:
            case HDUAL_VARIANT_PLAIN:
                iterate();
                break;
            case HDUAL_VARIANT_TASKS:
                iterate_tasks();
                break;
            case HDUAL_VARIANT_MULTI:
                iterate_multi();
                break;
            }
            if (invertHint)
                break;
        }
        if (model->countUpdate == 0)
            break;
    }
    if (dualInfeasCount > 0) {
        // We got free variables - need dual phase 1
        model->printMessage("dual-phase-2-found-free");
        solvePhase = 1;
    } else if (rowOut == -1) {
        // Tentative optimal
        model->printMessage("dual-phase-2-optimal");
        cleanup();
        if (dualInfeasCount > 0) {
            solvePhase = 4; // Do primal
        } else {
            solvePhase = 0;
            model->printMessage("problem-optimal");
            model->reportStatus(LP_Status_Optimal);
        }
    } else if (columnIn == -1) {
        model->printMessage("dual-phase-2-unbounded");
        if (model->problemPerturbed) {
            cleanup();
        } else {
            solvePhase = -1;
            model->printMessage("problem-infeasible");
            model->reportStatus(LP_Status_Infeasible);
        }
    }
}
void HDual::rebuild() {
    // Save history information
    model->reportPivots(-1, -1, 0); // Indicate REINVERT
    invertHint = 0;
    // Rebuild model->factor
    if (model->countUpdate > 0) {
        const int *baseIndex = model->getBaseIndex();
        for (int i = 0; i < numRow; i++)
            dualRHS.workDevexFull[baseIndex[i]] = dualRHS.workDevex[i];
        model->computeFactor();
        for (int i = 0; i < numRow; i++)
            dualRHS.workDevex[i] = dualRHS.workDevexFull[baseIndex[i]];
    }
    // Recompute dual solution
    model->computeDual();
    model->correctDual(&dualInfeasCount);
    model->computePrimal();
    // Collect primal infeasible as a list
    dualRHS.create_infeasArray();
    dualRHS.create_infeasList(columnDensity);
    // Compute the objective value
    model->computeObject(solvePhase);
    model->printObject();
    total_INVERT_TICK = factor->pseudoTick;
    total_FT_inc_TICK = 0;
    total_fake = 0;
}
void HDual::cleanup() {
    // Remove perturbation and recompute the dual solution
    model->printMessage("dual-cleanup-shift");
    model->initCost();
    model->initBound();
    model->computeDual();
    model->computeObject(solvePhase);
    model->printObject();
    model->computeDualInfeasInPrimal(&dualInfeasCount);
}
void HDual::iterate() {
    chooseRow();
    chooseColumn(&row_ep);
    updateFtranBFRT();
    updateFtran();
    updateFtranDSE(&row_ep);
    updateVerify();
    updateDual();
    updatePrimal(&row_ep);
    updatePivots();
}
void HDual::iterate_tasks() {
    slice_PRICE = 1;
    // Group 1
    chooseRow();
    // Disable slice when too sparse
    if (1.0 * row_ep.count / numRow < 0.01)
        slice_PRICE = 0;
// #pragma omp parallel
// #pragma omp single
    {
// #pragma omp task
        {
            columnDSE.copy(&row_ep);
            updateFtranDSE(&columnDSE);
        }
// #pragma omp task
        {
            if (slice_PRICE)
                chooseColumn_slice(&row_ep);
            else
                chooseColumn(&row_ep);
// #pragma omp task
            updateFtranBFRT();
// #pragma omp task
            updateFtran();
// #pragma omp taskwait
        }
    }
    updateVerify();
    updateDual();
    updatePrimal(&columnDSE);
    updatePivots();
}
void HDual::chooseRow() {
    if (invertHint)
        return;
    for (;;) {
        // Choose row
        dualRHS.choose_normal(&rowOut);
        if (rowOut == -1) {
            invertHint = 1;
            return;
        }
        // Verify weight
        row_ep.clear();
        row_ep.count = 1;
        row_ep.index[0] = rowOut;
        row_ep.array[rowOut] = 1;
        row_ep.packFlag = true;
        factor->btran(row_ep, row_epDensity);
        double u_weight = dualRHS.workDevex[rowOut];
        double c_weight = dualRHS.workDevex[rowOut] = row_ep.norm2();
        if (u_weight >= 0.25 * c_weight)
            break;
    }
    // Assign basic info
    columnOut = model->getBaseIndex()[rowOut];
    if (baseValue[rowOut] < baseLower[rowOut])
        deltaPrimal = baseValue[rowOut] - baseLower[rowOut];
    else
        deltaPrimal = baseValue[rowOut] - baseUpper[rowOut];
    sourceOut = deltaPrimal < 0 ? -1 : 1;
    row_epDensity *= 0.95;
    row_epDensity += 0.05 * row_ep.count / numRow;
}
void HDual::chooseColumn(HVector *row_ep) {
    if (invertHint)
        return;
    // Compute pivot row
    row_ap.clear();
    matrix->price_by_row(row_ap, *row_ep);
    // Choose column - possible
    dualRow.clear();
    dualRow.workDelta = deltaPrimal;
    dualRow.create_Freemove(row_ep);
    dualRow.choose_makepack(&row_ap, 0);
    dualRow.choose_makepack(row_ep, numCol);
    dualRow.choose_possible();
    // Choose column - check problem
    columnIn = -1;
    if (dualRow.workTheta <= 0 || dualRow.workCount == 0) {
        invertHint = 1;
        return;
    }
    // Choose column - final
    dualRow.choose_final();
    dualRow.delete_Freemove();
    columnIn = dualRow.workPivot;
    alphaRow = dualRow.workAlpha;
    thetaDual = dualRow.workTheta;
}
void HDual::chooseColumn_slice(HVector *row_ep) {
    if (invertHint)
        return;
    dualRow.clear();
    dualRow.workDelta = deltaPrimal;
    dualRow.create_Freemove(row_ep);
    // Row_ep:         PACK + CC1
// #pragma omp task
    {
        dualRow.choose_makepack(row_ep, numCol);
        dualRow.choose_possible();
    }
    // Row_ap: PRICE + PACK + CC1
    for (int i = 0; i < slice_num; i++) {
// #pragma omp task
        {
            slice_row_ap[i].clear();
            slice_matrix[i].price_by_row(slice_row_ap[i], *row_ep);
            slice_dualRow[i].clear();
            slice_dualRow[i].workDelta = deltaPrimal;
            slice_dualRow[i].choose_makepack(&slice_row_ap[i], slice_start[i]);
            slice_dualRow[i].choose_possible();
        }
    }
// #pragma omp taskwait
    // Join CC1 results here
    for (int i = 0; i < slice_num; i++)
        dualRow.choose_joinpack(&slice_dualRow[i]);
    // Infeasible we created before
    columnIn = -1;
    if (dualRow.workTheta <= 0 || dualRow.workCount == 0) {
        invertHint = 1;
        return;
    }
    // Choose column 2, This only happens if didn't go out
    dualRow.choose_final();
    dualRow.delete_Freemove();
    columnIn = dualRow.workPivot;
    alphaRow = dualRow.workAlpha;
    thetaDual = dualRow.workTheta;
}
void HDual::updateFtranBFRT() {
    if (invertHint)
        return;
    dualRow.update_flip(&columnBFRT);
    if (columnBFRT.count)
        factor->ftran(columnBFRT, columnDensity);
}
void HDual::updateFtran() {
    if (invertHint)
        return;
    column.clear();
    column.packFlag = true;
    matrix->collect_aj(column, columnIn, 1);
    factor->ftran(column, columnDensity);
    alpha = column.array[rowOut];
}
void HDual::updateFtranDSE(HVector *dseVector) {
    if (invertHint)
        return;
    factor->ftran(*dseVector, rowdseDensity);
}
void HDual::updateVerify() {
    if (invertHint)
        return;
    // The alpha
    double aCol = fabs(alpha);
    double aRow = fabs(alphaRow);
    double aDiff = fabs(aCol - aRow);
    if (aDiff / min(aCol, aRow) > 1e-7 && model->countUpdate > 0) {
        invertHint = 1;
    }
    // We get this thing, but it is not actived by default.
//    // The dual reduced cost
//    double dualin_u = workDual[columnIn];
//    double dualin_c = model->getWorkCost()[columnIn];
//    for (int i = 0; i < column.count; i++) {
//        int iRow = column.index[i];
//        int iCol = model->getBaseIndex()[iRow];
//        double value = column.array[iRow];
//        double cost = model->getWorkCost()[iCol] + model->getWorkShift()[iCol];
//        dualin_c -= cost * value;
//    }
//    double dualin_diff = fabs(dualin_c - dualin_u);
////    if (dualin_diff > Td) {
////        cout << dualin_c << "\t" << dualin_u << "\t" << dualin_diff << endl;
////        invertHint = 1;
////    }
}
void HDual::updateDual() {
    if (invertHint)
        return;
    // Update - dual (shift and back)
    if (thetaDual == 0)
        model->shiftCost(columnIn, -workDual[columnIn]);
    else {
        dualRow.update_dual(thetaDual);
        if (dual_variant != HDUAL_VARIANT_PLAIN && slice_PRICE)
            for (int i = 0; i < slice_num; i++)
                slice_dualRow[i].update_dual(thetaDual);
    }
    workDual[columnIn] = 0;
    workDual[columnOut] = -thetaDual;
    model->shiftBack(columnOut);
}
void HDual::updatePrimal(HVector *dseVector) {
    if (invertHint)
        return;
    // Update - primal and weight
    dualRHS.update_primal(&columnBFRT, 1);
    dualRHS.update_infeasList(&columnBFRT);
    double x_out = baseValue[rowOut];
    double l_out = baseLower[rowOut];
    double u_out = baseUpper[rowOut];
    thetaPrimal = (x_out - (deltaPrimal < 0 ? l_out : u_out)) / alpha;
    dualRHS.update_primal(&column, thetaPrimal);
    double thisDevex = dualRHS.workDevex[rowOut] / (alpha * alpha);
    dualRHS.update_weight(&column, thisDevex, -2 / alpha, &dseVector->array[0]);
    dualRHS.workDevex[rowOut] = thisDevex;
    dualRHS.update_infeasList(&column);
    rowdseDensity = 0.95 * rowdseDensity + 0.05 * dseVector->count / numRow;
    columnDensity = 0.95 * columnDensity + 0.05 * column.count / numRow;
    total_fake += column.fakeTick;
    total_fake += dseVector->fakeTick;
    total_FT_inc_TICK += column.pseudoTick;
    total_FT_inc_TICK += dseVector->pseudoTick;
}
void HDual::updatePivots() {
    if (invertHint)
        return;
    // Update - pivots
    model->updatePivots(columnIn, rowOut, sourceOut);
    model->reportPivots(columnIn, columnOut, alpha);
    model->updateFactor(&column, &row_ep, &rowOut, &invertHint);
    model->updateMatrix(columnIn, columnOut);
    dualRow.delete_Freelist(columnIn);
    dualRHS.update_pivots(rowOut,
            model->getWorkValue()[columnIn] + thetaPrimal);
    if (total_fake >= factor->fakeTick && model->countUpdate >= 50) {
//        cout << total_fake << "\t" << factor->fakeTick << endl;
//        printf(
//                "%10d   %10.2e  %10.2e  %10.4f          %10.2e  %10.2e  %10.4f\n",
//                model->countUpdate, total_INVERT_TICK, total_FT_inc_TICK,
//                total_FT_inc_TICK / total_INVERT_TICK, factor->fakeTick,
//                total_fake, total_fake / factor->fakeTick);
        invertHint = 1;
    }
}
// #include <iostream>
// #include <cstdio>
// #include <cassert>
// #include <cmath>
// #include <set>
// #include <stdexcept>
using namespace std;
void HDual::iterate_multi() {
    slice_PRICE = 1;
    // Report candidate
    major_chooseRow();
    minor_chooseRow();
    if (rowOut == -1) {
        invertHint = 1;
        return;
    }
    // Assign the slice_row_ep, skip if possible
    if (1.0 * multi_finish[multi_nFinish].row_ep->count / numRow < 0.01)
        slice_PRICE = 0;
    if (slice_PRICE) {
// #pragma omp parallel
// #pragma omp single
        chooseColumn_slice(multi_finish[multi_nFinish].row_ep);
    } else {
        chooseColumn(multi_finish[multi_nFinish].row_ep);
    }
    // If we failed.
    if (invertHint) {
        major_update();
        return;
    }
    minor_update();
    major_update();
}
void HDual::major_chooseRow() {
    /**
     * 0. Initial check to see if we need to do it again
     */
    if (model->countUpdate == 0)
        multi_chooseAgain = 1;
    if (!multi_chooseAgain)
        return;
    multi_chooseAgain = 0;
    multi_iteration++;
    /**
     * Major loop:
     *     repeat 1-5, until we found a good sets of choices
     */
    for (;;) {
        // 1. Multiple CHUZR
        int initialCount = 0;
        int choiceIndex[multi_num];
        dualRHS.choose_multi_HGauto(&choiceIndex[0], &initialCount, multi_num);
//        dualRHS.choose_multi_global(&choiceIndex[0], &initialCount, multi_num);
        if (initialCount == 0 && dualRHS.workCutoff == 0) {
            // OPTIMAL
            return;
        }
        // 2. Shrink the size by cutoff
        int choiceCount = 0;
        for (int i = 0; i < initialCount; i++) {
            int iRow = choiceIndex[i];
            if (dualRHS.workArray[iRow] / dualRHS.workDevex[iRow]
                    >= dualRHS.workCutoff) {
                choiceIndex[choiceCount++] = iRow;
            }
        }
        if (initialCount == 0 || choiceCount <= initialCount / 3) {
            // Need to do the list again
            dualRHS.create_infeasList(columnDensity);
            continue;
        }
        // 3. Store the choiceIndex to buffer
        for (int ich = 0; ich < multi_num; ich++)
            multi_choice[ich].rowOut = -1;
        for (int ich = 0; ich < choiceCount; ich++)
            multi_choice[ich].rowOut = choiceIndex[ich];
        // 4. Parallel BTRAN and compute weight
        major_chooseRowBtran();
        // 5. Update row weights
        for (int ich = 0; ich < multi_num; ich++) {
            if (multi_choice[ich].rowOut >= 0) {
                row_epDensity *= 0.95;
                row_epDensity += 0.05 * multi_choice[ich].row_ep.count / numRow;
            }
        }
        // 6. Check updated and computed weight
        int countWrongDevex = 0;
        for (int i = 0; i < multi_num; i++) {
            const int iRow = multi_choice[i].rowOut;
            if (iRow < 0)
                continue;
            double u_weight = dualRHS.workDevex[iRow];
            double c_weight = dualRHS.workDevex[iRow] =
                    multi_choice[i].infeasDevex;
            if (u_weight < 0.25 * c_weight) {
                multi_choice[i].rowOut = -1;
                countWrongDevex++;
            }
        }
        if (countWrongDevex <= choiceCount / 3)
            break;
    }
    // 6. Take other info associated with choices
    double pamiCutoff = model->dblOption[DBLOPT_PAMI_CUTOFF];
    for (int i = 0; i < multi_num; i++) {
        const int iRow = multi_choice[i].rowOut;
        if (iRow < 0)
            continue;
        // Other info
        multi_choice[i].baseValue = baseValue[iRow];
        multi_choice[i].baseLower = baseLower[iRow];
        multi_choice[i].baseUpper = baseUpper[iRow];
        multi_choice[i].infeasValue = dualRHS.workArray[iRow];
        multi_choice[i].infeasDevex = dualRHS.workDevex[iRow];
        multi_choice[i].infeasLimit = dualRHS.workArray[iRow]
                / dualRHS.workDevex[iRow];
        multi_choice[i].infeasLimit *= pamiCutoff;
    }
    // 6. Finish count
    multi_nFinish = 0;
}
void HDual::major_chooseRowBtran() {
    // 4.1. Prepare BTRAN buffer
    int multi_ntasks = 0;
    int multi_iRow[HSOL_THREAD_LIMIT];
    int multi_iwhich[HSOL_THREAD_LIMIT];
    double multi_devex[HSOL_THREAD_LIMIT];
    HVector_ptr multi_vector[HSOL_THREAD_LIMIT];
    for (int ich = 0; ich < multi_num; ich++) {
        if (multi_choice[ich].rowOut >= 0) {
            multi_iRow[multi_ntasks] = multi_choice[ich].rowOut;
            multi_vector[multi_ntasks] = &multi_choice[ich].row_ep;
            multi_iwhich[multi_ntasks] = ich;
            multi_ntasks++;
        }
    }
    // 4.2 Perform BTRAN
// #pragma omp parallel for schedule(static,1)
    for (int i = 0; i < multi_ntasks; i++) {
        const int iRow = multi_iRow[i];
        HVector_ptr work_ep = multi_vector[i];
        work_ep->clear();
        work_ep->count = 1;
        work_ep->index[0] = iRow;
        work_ep->array[iRow] = 1;
        work_ep->packFlag = true;
        factor->btran(*work_ep, row_epDensity);
        multi_devex[i] = work_ep->norm2();
    }
    // 4.3 Put back devex
    for (int i = 0; i < multi_ntasks; i++)
        multi_choice[multi_iwhich[i]].infeasDevex = multi_devex[i];
}
void HDual::minor_chooseRow() {
    /**
     * 1. Find which to go out
     *        Because we had other checking code
     *        We know current best is OK to be used
     */
    multi_iChoice = -1;
    double bestMerit = 0;
    for (int ich = 0; ich < multi_num; ich++) {
        const int iRow = multi_choice[ich].rowOut;
        if (iRow < 0)
            continue;
        double infeasValue = multi_choice[ich].infeasValue;
        double infeasDevex = multi_choice[ich].infeasDevex;
        double infeasMerit = infeasValue / infeasDevex;
        if (bestMerit < infeasMerit) {
            bestMerit = infeasMerit;
            multi_iChoice = ich;
        }
    }
    /**
     * 2. Obtain other info for
     *        current sub-optimisation choice
     */
    rowOut = -1;
    if (multi_iChoice != -1) {
        MChoice *workChoice = &multi_choice[multi_iChoice];
        // Assign useful variables
        rowOut = workChoice->rowOut;
        columnOut = model->getBaseIndex()[rowOut];
        double valueOut = workChoice->baseValue;
        double lowerOut = workChoice->baseLower;
        double upperOut = workChoice->baseUpper;
        deltaPrimal = valueOut - (valueOut < lowerOut ? lowerOut : upperOut);
        sourceOut = deltaPrimal < 0 ? -1 : 1;
        // Assign buffers
        MFinish *workFinish = &multi_finish[multi_nFinish];
        workFinish->rowOut = rowOut;
        workFinish->columnOut = columnOut;
        workFinish->row_ep = &workChoice->row_ep;
        workFinish->column = &workChoice->column;
        workFinish->columnBFRT = &workChoice->columnBFRT;
        workFinish->devex = workChoice->infeasDevex;
        // Disable current row
        workChoice->rowOut = -1;
    }
}
void HDual::minor_update() {
    // Minor update - store roll back data
    MFinish *Fin = &multi_finish[multi_nFinish];
    Fin->moveIn = model->getNonbasicMove()[columnIn];
    Fin->shiftOut = model->getWorkShift()[columnOut];
    Fin->flipList.clear();
    for (int i = 0; i < dualRow.workCount; i++)
        Fin->flipList.push_back(dualRow.workData[i].first);
    // Minor update - key parts
    minor_updateDual();
    minor_updatePrimal();
    minor_updatePivots();
    minor_updateRows();
    multi_nFinish++;
    // Minor update - check for the next iteration
    int countRemain = 0;
    for (int i = 0; i < multi_num; i++) {
        int iRow = multi_choice[i].rowOut;
        if (iRow < 0)
            continue;
        double myInfeas = multi_choice[i].infeasValue;
        double myWeight = multi_choice[i].infeasDevex;
        countRemain += (myInfeas / myWeight > multi_choice[i].infeasLimit);
    }
    if (countRemain == 0)
        multi_chooseAgain = 1;
//    if (multi_nFinish + 1 == multi_num)
//        multi_chooseAgain = 1;
}
void HDual::minor_updateDual() {
    /**
     * 1. Update the dual solution
     *    XXX Data parallel (depends on the ap partition before)
     */
    if (thetaDual == 0) {
        model->shiftCost(columnIn, -workDual[columnIn]);
    } else {
        dualRow.update_dual(thetaDual);
        if (slice_PRICE) {
            for (int i = 0; i < slice_num; i++)
                slice_dualRow[i].update_dual(thetaDual);
        }
    }
    workDual[columnIn] = 0;
    workDual[columnOut] = -thetaDual;
    model->shiftBack(columnOut);
    /**
     * 2. Apply global bound flip
     */
    dualRow.update_flip(multi_finish[multi_nFinish].columnBFRT);
    /**
     * 3. Apply local bound flips
     */
    for (int ich = 0; ich < multi_num; ich++) {
        if (ich == multi_iChoice || multi_choice[ich].rowOut >= 0) {
            HVector *this_ep = &multi_choice[ich].row_ep;
            for (int i = 0; i < dualRow.workCount; i++) {
                double dot = matrix->compute_dot(*this_ep,
                        dualRow.workData[i].first);
                multi_choice[ich].baseValue -= dualRow.workData[i].second * dot;
            }
        }
    }
}
void HDual::minor_updatePrimal() {
    MChoice *Cho = &multi_choice[multi_iChoice];
    MFinish *Fin = &multi_finish[multi_nFinish];
    double valueOut = Cho->baseValue;
    double lowerOut = Cho->baseLower;
    double upperOut = Cho->baseUpper;
    if (deltaPrimal < 0) {
        thetaPrimal = (valueOut - lowerOut) / alphaRow;
        Fin->basicBound = lowerOut;
    }
    if (deltaPrimal > 0) {
        thetaPrimal = (valueOut - upperOut) / alphaRow;
        Fin->basicBound = upperOut;
    }
    Fin->thetaPrimal = thetaPrimal;
    /**
     * 5. Update the other primal value
     *    By the pivot (thetaPrimal)
     */
    for (int ich = 0; ich < multi_num; ich++) {
        if (multi_choice[ich].rowOut >= 0) {
            HVector *this_ep = &multi_choice[ich].row_ep;
            double dot = matrix->compute_dot(*this_ep, columnIn);
            multi_choice[ich].baseValue -= thetaPrimal * dot;
            double value = multi_choice[ich].baseValue;
            double lower = multi_choice[ich].baseLower;
            double upper = multi_choice[ich].baseUpper;
            double infeas = 0;
            if (value < lower - Tp)
                infeas = value - lower;
            if (value > upper + Tp)
                infeas = value - upper;
            infeas *= infeas;
            multi_choice[ich].infeasValue = infeas;
        }
    }
}
void HDual::minor_updatePivots() {
    MFinish *Fin = &multi_finish[multi_nFinish];
    model->updatePivots(columnIn, rowOut, sourceOut);
    Fin->devex /= (alphaRow * alphaRow);
    Fin->basicValue = model->getWorkValue()[columnIn] + thetaPrimal;
    model->updateMatrix(columnIn, columnOut);
    Fin->columnIn = columnIn;
    Fin->alphaRow = alphaRow;
    model->reportPivots(columnIn, columnOut, alphaRow);
}
void HDual::minor_updateRows() {
    const HVector *Row = multi_finish[multi_nFinish].row_ep;
    int updateRows_inDense = (Row->count < 0) || (Row->count > 0.1 * numRow);
    if (updateRows_inDense) {
        int multi_nTasks = 0;
        int multi_iwhich[HSOL_THREAD_LIMIT];
        double multi_xpivot[HSOL_THREAD_LIMIT];
        HVector_ptr multi_vector[HSOL_THREAD_LIMIT];
        /*
         * Dense mode
         *  1. Find which ones to do and the pivotX
         *  2. Do all of them in task parallel
         */
        // Collect tasks
        for (int ich = 0; ich < multi_num; ich++) {
            if (multi_choice[ich].rowOut >= 0) {
                HVector *next_ep = &multi_choice[ich].row_ep;
                double pivotX = matrix->compute_dot(*next_ep, columnIn);
                if (fabs(pivotX) < HSOL_CONST_TINY)
                    continue;
                multi_vector[multi_nTasks] = next_ep;
                multi_xpivot[multi_nTasks] = -pivotX / alphaRow;
                multi_iwhich[multi_nTasks] = ich;
                multi_nTasks++;
            }
        }
        // Perform tasks
// #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < multi_nTasks; i++) {
            HVector_ptr nextEp = multi_vector[i];
            const double xpivot = multi_xpivot[i];
            nextEp->saxpy(xpivot, Row);
            nextEp->tight();
            multi_xpivot[i] = nextEp->norm2();
        }
        // Put weight back
        for (int i = 0; i < multi_nTasks; i++)
            multi_choice[multi_iwhich[i]].infeasDevex = multi_xpivot[i];
    } else {
        // Sparse mode: just do it sequentially
        for (int ich = 0; ich < multi_num; ich++) {
            if (multi_choice[ich].rowOut >= 0) {
                HVector *next_ep = &multi_choice[ich].row_ep;
                double pivotX = matrix->compute_dot(*next_ep, columnIn);
                if (fabs(pivotX) < HSOL_CONST_TINY)
                    continue;
                next_ep->saxpy(-pivotX / alphaRow, Row);
                next_ep->tight();
                multi_choice[ich].infeasDevex = next_ep->norm2();
            }
        }
    }
}
void HDual::major_update() {
    /**
     * 0. See if it's ready to perform a major update
     */
    if (invertHint)
        multi_chooseAgain = 1;
    if (!multi_chooseAgain)
        return;
    // Major update - FTRANs
    major_updateFtranPrepare();
    major_updateFtranParallel();
    major_updateFtranFinal();
    // Major update - check for roll back
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        MFinish *iFinish = &multi_finish[iFn];
        HVector *iColumn = iFinish->column;
        int iRowOut = iFinish->rowOut;
        double alphaC = fabs(iColumn->array[iRowOut]);
        double alphaR = fabs(iFinish->alphaRow);
        double compare = min(alphaC, alphaR);
        double alphaDiff = fabs(alphaC - alphaR);
//        int startUpdate = model->countUpdate - multi_nFinish;
        if (alphaDiff / compare > 1e-8 && model->countUpdate > 0) {
            cout << "REPORT " << model->modelName << " NEED-ROLL-BACK   ";
            cout << model->numberIteration << " alpha = " << alphaC
                    << " alphaR = " << alphaR << " diff = "
                    << alphaDiff / compare << "  multi_nFinish = "
                    << multi_nFinish << endl;
            invertHint = 1;
//            if (startUpdate > 0) {
                major_rollback();
                return;
//            }
        }
    }
    // Major update - primal and factor
    major_updatePrimal();
    major_updateFactor();
}
void HDual::major_updateFtranPrepare() {
    // Prepare FTRAN BFRT buffer
    columnBFRT.clear();
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        MFinish *Fin = &multi_finish[iFn];
        HVector *Vec = Fin->columnBFRT;
        matrix->collect_aj(*Vec, Fin->columnIn, Fin->thetaPrimal);
        // Update this buffer by previous Row_ep
        for (int jFn = iFn - 1; jFn >= 0; jFn--) {
            MFinish *jFinish = &multi_finish[jFn];
            double *jRow_epArray = &jFinish->row_ep->array[0];
            double pivotX = 0;
            for (int k = 0; k < Vec->count; k++) {
                int iRow = Vec->index[k];
                pivotX += Vec->array[iRow] * jRow_epArray[iRow];
            }
            if (fabs(pivotX) > HSOL_CONST_TINY) {
                pivotX /= jFinish->alphaRow;
                matrix->collect_aj(*Vec, jFinish->columnIn, -pivotX);
                matrix->collect_aj(*Vec, jFinish->columnOut, pivotX);
            }
        }
        columnBFRT.saxpy(1, Vec);
    }
    // Prepare regular FTRAN buffer
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        MFinish *iFinish = &multi_finish[iFn];
        HVector *iColumn = iFinish->column;
        iColumn->clear();
        iColumn->packFlag = true;
        matrix->collect_aj(*iColumn, iFinish->columnIn, 1);
    }
}
void HDual::major_updateFtranParallel() {
    // Prepare buffers
    int multi_ntasks = 0;
    double multi_density[HSOL_THREAD_LIMIT * 2 + 1];
    HVector_ptr multi_vector[HSOL_THREAD_LIMIT * 2 + 1];
    // BFRT first
    multi_density[multi_ntasks] = columnDensity;
    multi_vector[multi_ntasks] = &columnBFRT;
    multi_ntasks++;
    // Then DSE
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        multi_density[multi_ntasks] = rowdseDensity;
        multi_vector[multi_ntasks] = multi_finish[iFn].row_ep;
        multi_ntasks++;
    }
    // Then Column
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        multi_density[multi_ntasks] = columnDensity;
        multi_vector[multi_ntasks] = multi_finish[iFn].column;
        multi_ntasks++;
    }
    // Perform FTRAN
// #pragma omp parallel for schedule(dynamic, 1)
    for (int i = 0; i < multi_ntasks; i++) {
        HVector_ptr rhs = multi_vector[i];
        double density = multi_density[i];
        factor->ftran(*rhs, density);
    }
    // Update ticks
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        MFinish *Fin = &multi_finish[iFn];
        HVector *Col = Fin->column;
        HVector *Row = Fin->row_ep;
        total_FT_inc_TICK += Col->pseudoTick;
        total_FT_inc_TICK += Row->pseudoTick;
    }
    // Update rates
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        MFinish *Fin = &multi_finish[iFn];
        HVector *Col = Fin->column;
        HVector *Row = Fin->row_ep;
        columnDensity = 0.95 * columnDensity + 0.05 * Col->count / numRow;
        rowdseDensity = 0.95 * rowdseDensity + 0.05 * Row->count / numRow;
    }
}
void HDual::major_updateFtranFinal() {
    int updateFTRAN_inDense = dualRHS.workCount < 0;
    if (updateFTRAN_inDense) {
        for (int iFn = 0; iFn < multi_nFinish; iFn++) {
            multi_finish[iFn].column->count = -1;
            multi_finish[iFn].row_ep->count = -1;
            double *myCol = &multi_finish[iFn].column->array[0];
            double *myRow = &multi_finish[iFn].row_ep->array[0];
            for (int jFn = 0; jFn < iFn; jFn++) {
                int pivotRow = multi_finish[jFn].rowOut;
                const double pivotAlpha = multi_finish[jFn].alphaRow;
                const double *pivotArray = &multi_finish[jFn].column->array[0];
                double pivotX1 = myCol[pivotRow];
                double pivotX2 = myRow[pivotRow];
                // The FTRAN regular buffer
                if (fabs(pivotX1) > HSOL_CONST_TINY) {
                    const double pivot = pivotX1 / pivotAlpha;
// #pragma omp parallel for
                    for (int i = 0; i < numRow; i++)
                        myCol[i] -= pivot * pivotArray[i];
                    myCol[pivotRow] = pivot;
                }
                // The FTRAN-DSE buffer
                if (fabs(pivotX2) > HSOL_CONST_TINY) {
                    const double pivot = pivotX2 / pivotAlpha;
// #pragma omp parallel for
                    for (int i = 0; i < numRow; i++)
                        myRow[i] -= pivot * pivotArray[i];
                    myRow[pivotRow] = pivot;
                }
            }
        }
    } else {
        for (int iFn = 0; iFn < multi_nFinish; iFn++) {
            MFinish *Fin = &multi_finish[iFn];
            HVector *Col = Fin->column;
            HVector *Row = Fin->row_ep;
            for (int jFn = 0; jFn < iFn; jFn++) {
                MFinish *jFinish = &multi_finish[jFn];
                int pivotRow = jFinish->rowOut;
                double pivotX1 = Col->array[pivotRow];
                // The FTRAN regular buffer
                if (fabs(pivotX1) > HSOL_CONST_TINY) {
                    pivotX1 /= jFinish->alphaRow;
                    Col->saxpy(-pivotX1, jFinish->column);
                    Col->array[pivotRow] = pivotX1;
                }
                // The FTRAN-DSE buffer
                double pivotX2 = Row->array[pivotRow];
                if (fabs(pivotX2) > HSOL_CONST_TINY) {
                    pivotX2 /= jFinish->alphaRow;
                    Row->saxpy(-pivotX2, jFinish->column);
                    Row->array[pivotRow] = pivotX2;
                }
            }
        }
    }
}
void HDual::major_updatePrimal() {
    int updatePrimal_inDense = dualRHS.workCount < 0;
    if (updatePrimal_inDense) {
        // Update the RHS in dense
        const double *mixArray = &columnBFRT.array[0];
        double *rhs = &dualRHS.workArray[0];
// #pragma omp parallel for schedule(static)
        for (int iRow = 0; iRow < numRow; iRow++) {
            baseValue[iRow] -= mixArray[iRow];
            const double value = baseValue[iRow];
            const double less = baseLower[iRow] - value;
            const double more = value - baseUpper[iRow];
            double infeas = less > Tp ? less : (more > Tp ? more : 0);
            rhs[iRow] = infeas * infeas;
        }
        // Update the weight in dense
        for (int iFn = 0; iFn < multi_nFinish; iFn++) {
            const double pivotDevex = multi_finish[iFn].devex;
            const double *colArray = &multi_finish[iFn].column->array[0];
            const double *dseArray = &multi_finish[iFn].row_ep->array[0];
            double Kai = -2 / multi_finish[iFn].alphaRow;
            double *devex = &dualRHS.workDevex[0];
// #pragma omp parallel for schedule(static)
            for (int iRow = 0; iRow < numRow; iRow++) {
                const double val = colArray[iRow];
                devex[iRow] += val * (pivotDevex * val + Kai * dseArray[iRow]);
                if (devex[iRow] < 1e-4)
                    devex[iRow] = 1e-4;
            }
        }
    } else {
        // Update primal and pivots
        dualRHS.update_primal(&columnBFRT, 1);
        dualRHS.update_infeasList(&columnBFRT);
        // Update weights
        for (int iFn = 0; iFn < multi_nFinish; iFn++) {
            MFinish *Fin = &multi_finish[iFn];
            HVector *Col = Fin->column;
            HVector *Row = Fin->row_ep;
            double Kai = -2 / Fin->alphaRow;
            dualRHS.update_weight(Col, Fin->devex, Kai, &Row->array[0]);
            dualRHS.update_infeasList(Col);
        }
    }
    // Update primal value for the pivots
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        MFinish *Fin = &multi_finish[iFn];
        int iRow = Fin->rowOut;
        double value = baseValue[iRow] - Fin->basicBound + Fin->basicValue;
        dualRHS.update_pivots(iRow, value);
    }
    // Update weight value for the pivots
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        const int iRow = multi_finish[iFn].rowOut;
        const double pivotDevex = multi_finish[iFn].devex;
        const double *colArray = &multi_finish[iFn].column->array[0];
        const double *dseArray = &multi_finish[iFn].row_ep->array[0];
        double Kai = -2 / multi_finish[iFn].alphaRow;
        for (int jFn = 0; jFn < iFn; jFn++) {
            int jRow = multi_finish[jFn].rowOut;
            double value = colArray[jRow];
            double devex = dualRHS.workDevex[jRow];
            devex += value * (pivotDevex * value + Kai * dseArray[jRow]);
            if (devex < 1e-4)
                devex = 1e-4;
            dualRHS.workDevex[jRow] = devex;
        }
        dualRHS.workDevex[iRow] = pivotDevex;
    }
}
void HDual::major_updateFactor() {
    /**
     * 9. Update the factor by CFT
     */
    int iRows[multi_nFinish];
    for (int iCh = 0; iCh < multi_nFinish - 1; iCh++) {
        multi_finish[iCh].row_ep->next = multi_finish[iCh + 1].row_ep;
        multi_finish[iCh].column->next = multi_finish[iCh + 1].column;
        iRows[iCh] = multi_finish[iCh].rowOut;
    }
    iRows[multi_nFinish - 1] = multi_finish[multi_nFinish - 1].rowOut;
    if (multi_nFinish > 0)
        model->updateFactor(multi_finish[0].column, multi_finish[0].row_ep,
                iRows, &invertHint);
    if (total_FT_inc_TICK > total_INVERT_TICK * 1.5 && model->countUpdate > 200)
        invertHint = 1;
}
void HDual::major_rollback() {
    for (int iFn = multi_nFinish - 1; iFn >= 0; iFn--) {
        MFinish *Fin = &multi_finish[iFn];
        // 1. Roll back pivot
        model->getNonbasicMove()[Fin->columnIn] = Fin->moveIn;
        model->getNonbasicFlag()[Fin->columnIn] = 1;
        model->getNonbasicMove()[Fin->columnOut] = 0;
        model->getNonbasicFlag()[Fin->columnOut] = 0;
        model->getBaseIndex()[Fin->rowOut] = Fin->columnOut;
        // 2. Roll back matrix
        model->updateMatrix(Fin->columnOut, Fin->columnIn);
        // 3. Roll back flips
        for (unsigned i = 0; i < Fin->flipList.size(); i++)
            model->flipBound(Fin->flipList[i]);
        // 4. Roll back cost
        model->getWorkShift()[Fin->columnIn] = 0;
        model->getWorkShift()[Fin->columnOut] = Fin->shiftOut;
        // 5. The iteration count
        model->numberIteration--;
    }
}
// #include <iostream>
// #include <fstream>
// #include <algorithm>
// #include <set>
using namespace std;
void HDualRHS::setup(HModel *model) {
    workModel = model;
    workMark.resize(workModel->getNumRow());
    workIndex.resize(workModel->getNumRow());
    workArray.resize(workModel->getNumRow());
    workDevex.assign(workModel->getNumRow(), 1);
    workDevexFull.resize(workModel->getNumTot());
    partNum = 0;
    partSwitch = 0;
}
void HDualRHS::setup_partition(const char *filename) {
    ifstream reader(filename);
    reader >> partNum >> partNumRow >> partNumCol >> partNumCut;
    if (partNumRow != workModel->getNumRow()) {
        partNum = 0;
        workModel->printMessage("wrong-partition-file");
        reader.close();
        return;
    }
    workPartition.assign(workModel->getNumRow(), 0);
    for (int i = 0; i < partNumRow; i++)
        reader >> workPartition[i];
    reader.close();
    partSwitch = 1;
}
void HDualRHS::choose_normal(int *chIndex) {
    int random = workModel->random.intRandom();
    if (workCount < 0) {
        // DENSE mode
        const int numRow = -workCount;
        int randomStart = random % numRow;
        double bestMerit = 0;
        int bestIndex = -1;
        for (int section = 0; section < 2; section++) {
            const int start = (section == 0) ? randomStart : 0;
            const int end = (section == 0) ? numRow : randomStart;
            for (int iRow = start; iRow < end; iRow++) {
                if (workArray[iRow] > HSOL_CONST_ZERO) {
                    const double myInfeas = workArray[iRow];
                    const double myWeight = workDevex[iRow];
                    if (bestMerit * myWeight < myInfeas) {
                        bestMerit = myInfeas / myWeight;
                        bestIndex = iRow;
                    }
                }
            }
        }
        *chIndex = bestIndex;
    } else {
        // SPARSE mode
        if (workCount == 0) {
            *chIndex = -1;
            return;
        }
        int randomStart = random % workCount;
        double bestMerit = 0;
        int bestIndex = -1;
        for (int section = 0; section < 2; section++) {
            const int start = (section == 0) ? randomStart : 0;
            const int end = (section == 0) ? workCount : randomStart;
            for (int i = start; i < end; i++) {
                int iRow = workIndex[i];
                if (workArray[iRow] > HSOL_CONST_ZERO) {
                    const double myInfeas = workArray[iRow];
                    const double myWeight = workDevex[iRow];
                    if (bestMerit * myWeight < myInfeas) {
                        bestMerit = myInfeas / myWeight;
                        bestIndex = iRow;
                    }
                }
            }
        }
        int createListAgain = 0;
        if (bestIndex == -1) {
            createListAgain = workCutoff > 0;
        } else if (bestMerit <= workCutoff * 0.99) {
            createListAgain = 1;
        }
        if (createListAgain) {
            create_infeasList(0);
            choose_normal(&bestIndex);
        }
        *chIndex = bestIndex;
    }
}
void HDualRHS::choose_multi_global(int *chIndex, int *chCount, int chLimit) {
    for (int i = 0; i < chLimit; i++)
        chIndex[i] = -1;
    const unsigned int chooseCHECK = chLimit * 2;
    vector<pair<double, int> > setP;
    setP.reserve(chooseCHECK);
    if (workCount < 0) {
        // DENSE mode
        const int numRow = -workCount;
        double cutoffMerit = 0;
        for (int iRow = 0; iRow < numRow; iRow++) {
            if (workArray[iRow] > HSOL_CONST_ZERO) {
                const double myInfeas = workArray[iRow];
                const double myWeight = workDevex[iRow];
                if (cutoffMerit * myWeight < myInfeas) {
                    // Save
                    setP.push_back(make_pair(-myInfeas / myWeight, iRow));
                    // Shrink
                    if (setP.size() >= chooseCHECK) {
                        sort(setP.begin(), setP.end());
                        setP.resize(chLimit);
                        cutoffMerit = -setP.back().first;
                    }
                }
            }
        }
    } else {
        // SPARSE Mode
        double cutoffMerit = 0;
        for (int i = 0; i < workCount; i++) {
            int iRow = workIndex[i];
            if (workArray[iRow] > HSOL_CONST_ZERO) {
                const double myInfeas = workArray[iRow];
                const double myWeight = workDevex[iRow];
                if (cutoffMerit * myWeight < myInfeas) {
                    // Save
                    setP.push_back(make_pair(-myInfeas / myWeight, iRow));
                    // Shrink
                    if (setP.size() >= chooseCHECK) {
                        sort(setP.begin(), setP.end());
                        setP.resize(chLimit);
                        cutoffMerit = -setP.back().first;
                    }
                }
            }
        }
    }
    // Store the setP
    sort(setP.begin(), setP.end());
    if ((int) (setP.size()) > chLimit)
        setP.resize(chLimit);
    *chCount = setP.size();
    for (unsigned i = 0; i < setP.size(); i++)
        chIndex[i] = setP[i].second;
}
void HDualRHS::choose_multi_HGauto(int *chIndex, int *chCount, int chLimit) {
    // Automatically decide to use partition or not
    if (partSwitch)
        choose_multi_HGpart(chIndex, chCount, chLimit);
    else
        choose_multi_global(chIndex, chCount, chLimit);
}
void HDualRHS::choose_multi_HGpart(int *chIndex, int *chCount, int chLimit) {
    // Force to use partition method, unless doesn't exist
    if (partNum != chLimit) {
        choose_multi_global(chIndex, chCount, chLimit);
        partSwitch = 0;
        return;
    }
    // Initialise
    for (int i = 0; i < chLimit; i++)
        chIndex[i] = -1;
    *chCount = 0;
    int random = workModel->random.intRandom();
    if (workCount < 0) {
        // DENSE mode
        const int numRow = -workCount;
        int randomStart = random % numRow;
        vector<double> bestMerit(chLimit, 0);
        vector<int> bestIndex(chLimit, -1);
        for (int section = 0; section < 2; section++) {
            const int start = (section == 0) ? randomStart : 0;
            const int end = (section == 0) ? numRow : randomStart;
            for (int iRow = start; iRow < end; iRow++) {
                if (workArray[iRow] > HSOL_CONST_ZERO) {
                    int iPart = workPartition[iRow];
                    const double myInfeas = workArray[iRow];
                    const double myWeight = workDevex[iRow];
                    if (bestMerit[iPart] * myWeight < myInfeas) {
                        bestMerit[iPart] = myInfeas / myWeight;
                        bestIndex[iPart] = iRow;
                    }
                }
            }
        }
        int count = 0;
        for (int i = 0; i < chLimit; i++) {
            if (bestIndex[i] != -1) {
                chIndex[count++] = bestIndex[i];
            }
        }
        *chCount = count;
    } else {
        // SPARSE mode
        if (workCount == 0) {
            return;
        }
        int randomStart = random % workCount;
        vector<double> bestMerit(chLimit, 0);
        vector<int> bestIndex(chLimit, -1);
        for (int section = 0; section < 2; section++) {
            const int start = (section == 0) ? randomStart : 0;
            const int end = (section == 0) ? workCount : randomStart;
            for (int i = start; i < end; i++) {
                int iRow = workIndex[i];
                if (workArray[iRow] > HSOL_CONST_ZERO) {
                    int iPart = workPartition[iRow];
                    const double myInfeas = workArray[iRow];
                    const double myWeight = workDevex[iRow];
                    if (bestMerit[iPart] * myWeight < myInfeas) {
                        bestMerit[iPart] = myInfeas / myWeight;
                        bestIndex[iPart] = iRow;
                    }
                }
            }
        }
        int count = 0;
        for (int i = 0; i < chLimit; i++) {
            if (bestIndex[i] != -1) {
                chIndex[count++] = bestIndex[i];
            }
        }
        *chCount = count;
    }
}
void HDualRHS::update_primal(HVector *column, double theta) {
    const int numRow = workModel->getNumRow();
    const int columnCount = column->count;
    const int *columnIndex = &column->index[0];
    const double *columnArray = &column->array[0];
    const double *baseLower = workModel->getBaseLower();
    const double *baseUpper = workModel->getBaseUpper();
    const double Tp = workModel->dblOption[DBLOPT_PRIMAL_TOL];
    double *baseValue = workModel->getBaseValue();
    bool updatePrimal_inDense = columnCount < 0 || columnCount > 0.4 * numRow;
    if (updatePrimal_inDense) {
        for (int iRow = 0; iRow < numRow; iRow++) {
            baseValue[iRow] -= theta * columnArray[iRow];
            const double value = baseValue[iRow];
            const double less = baseLower[iRow] - value;
            const double more = value - baseUpper[iRow];
            double infeas = less > Tp ? less : (more > Tp ? more : 0);
            workArray[iRow] = infeas * infeas;
        }
    } else {
        for (int i = 0; i < columnCount; i++) {
            int iRow = columnIndex[i];
            baseValue[iRow] -= theta * columnArray[iRow];
            const double value = baseValue[iRow];
            const double less = baseLower[iRow] - value;
            const double more = value - baseUpper[iRow];
            double infeas = less > Tp ? less : (more > Tp ? more : 0);
            workArray[iRow] = infeas * infeas;
        }
    }
}
void HDualRHS::update_weight(HVector *column, double devex, double Kai,
        double *dseArray) {
    const int numRow = workModel->getNumRow();
    const int columnCount = column->count;
    const int *columnIndex = &column->index[0];
    const double *columnArray = &column->array[0];
    bool updateWeight_inDense = columnCount < 0 || columnCount > 0.4 * numRow;
    if (updateWeight_inDense) {
        for (int iRow = 0; iRow < numRow; iRow++) {
            const double val = columnArray[iRow];
            workDevex[iRow] += val * (devex * val + Kai * dseArray[iRow]);
            if (workDevex[iRow] < 1e-4)
                workDevex[iRow] = 1e-4;
        }
    } else {
        for (int i = 0; i < columnCount; i++) {
            const int iRow = columnIndex[i];
            const double val = columnArray[iRow];
            workDevex[iRow] += val * (devex * val + Kai * dseArray[iRow]);
            if (workDevex[iRow] < 1e-4)
                workDevex[iRow] = 1e-4;
        }
    }
}
void HDualRHS::update_pivots(int iRow, double value) {
    const double *baseLower = workModel->getBaseLower();
    const double *baseUpper = workModel->getBaseUpper();
    const double Tp = workModel->dblOption[DBLOPT_PRIMAL_TOL];
    double *baseValue = workModel->getBaseValue();
    baseValue[iRow] = value;
    double pivotInfeas = 0;
    if (baseValue[iRow] < baseLower[iRow] - Tp)
        pivotInfeas = baseValue[iRow] - baseLower[iRow];
    if (baseValue[iRow] > baseUpper[iRow] + Tp)
        pivotInfeas = baseValue[iRow] - baseUpper[iRow];
    workArray[iRow] = pivotInfeas * pivotInfeas;
}
void HDualRHS::update_infeasList(HVector *column) {
    const int columnCount = column->count;
    const int *columnIndex = &column->index[0];
    // DENSE mode: disabled
    if (workCount < 0)
        return;
    if (workCutoff <= 0) {
        // The regular sparse way
        for (int i = 0; i < columnCount; i++) {
            int iRow = columnIndex[i];
            if (workMark[iRow] == 0) {
                if (workArray[iRow]) {
                    workIndex[workCount++] = iRow;
                    workMark[iRow] = 1;
                }
            }
        }
    } else {
        // The hyper sparse way
        for (int i = 0; i < columnCount; i++) {
            int iRow = columnIndex[i];
            if (workMark[iRow] == 0) {
                if (workArray[iRow] > workDevex[iRow] * workCutoff) {
                    workIndex[workCount++] = iRow;
                    workMark[iRow] = 1;
                }
            }
        }
    }
}
void HDualRHS::create_infeasArray() {
    int numRow = workModel->getNumRow();
    const double *baseValue = workModel->getBaseValue();
    const double *baseLower = workModel->getBaseLower();
    const double *baseUpper = workModel->getBaseUpper();
    const double Tp = workModel->dblOption[DBLOPT_PRIMAL_TOL];
    for (int i = 0; i < numRow; i++) {
        const double value = baseValue[i];
        const double less = baseLower[i] - value;
        const double more = value - baseUpper[i];
        double infeas = less > Tp ? less : (more > Tp ? more : 0);
        workArray[i] = infeas * infeas;
    }
}
void HDualRHS::create_infeasList(double columnDensity) {
    int numRow = workModel->getNumRow();
    double *dwork = &workDevexFull[0];
// 1. Build the full list
    fill_n(&workMark[0], numRow, 0);
    workCount = 0;
    workCutoff = 0;
    for (int iRow = 0; iRow < numRow; iRow++) {
        if (workArray[iRow]) {
            workMark[iRow] = 1;
            workIndex[workCount++] = iRow;
        }
    }
// 2. See if it worth to try to go sparse
//    (Many candidates, really sparse RHS)
    if (workCount > max(numRow * 0.01, 500.0) && columnDensity < 0.05) {
        int icutoff = max(workCount * 0.001, 500.0);
        double maxMerit = 0;
        for (int iRow = 0, iPut = 0; iRow < numRow; iRow++)
            if (workMark[iRow]) {
                double myMerit = workArray[iRow] / workDevex[iRow];
                if (maxMerit < myMerit)
                    maxMerit = myMerit;
                dwork[iPut++] = -myMerit;
            }
        nth_element(dwork, dwork + icutoff, dwork + workCount);
        double cutMerit = -dwork[icutoff];
        workCutoff = min(maxMerit * 0.99999, cutMerit * 1.00001);
        // Create again
        fill_n(&workMark[0], numRow, 0);
        workCount = 0;
        for (int iRow = 0; iRow < numRow; iRow++) {
            if (workArray[iRow] >= workDevex[iRow] * workCutoff) {
                workIndex[workCount++] = iRow;
                workMark[iRow] = 1;
            }
        }
        // Reduce by drop smaller
        if (workCount > icutoff * 1.5) {
            // Firstly take up "icutoff" number of elements
            int fullCount = workCount;
            workCount = icutoff;
            for (int i = icutoff; i < fullCount; i++) {
                int iRow = workIndex[i];
                if (workArray[iRow] > workDevex[iRow] * cutMerit) {
                    workIndex[workCount++] = iRow;
                } else {
                    workMark[iRow] = 0;
                }
            }
        }
//        cout
//                << "======================================================> WORK COUNT = "
//                << workCount << "\t icutoff = " << icutoff << "\t maxMerit = "
//                << maxMerit << "\t cutMerit = " << cutMerit << endl;
    }
// 3. If there is still too much candidates: disable them
    if (workCount > 0.2 * numRow) {
        workCount = -numRow;
        workCutoff = 0;
    }
}
// #include <cassert>
// #include <iostream>
using namespace std;
void HDualRow::setupSlice(HModel *model, int size) {
    // Copy pointer
    workModel = model;
    workSize = size;
    workMove = model->getNonbasicMove();
    workDual = model->getWorkDual();
    workRange = model->getWorkRange();
    // Allocate spaces
    packCount = 0;
    packIndex.resize(workSize);
    packValue.resize(workSize);
    workCount = 0;
    workData.resize(workSize);
}
void HDualRow::setup(HModel *model) {
    // Setup common vectors
    setupSlice(model, model->getNumTot());
    workRand = model->getWorkIntBreak();
}
void HDualRow::clear() {
    packCount = 0;
    workCount = 0;
}
void HDualRow::choose_makepack(const HVector *row, const int offset) {
    /**
     * Pack the index array to the row
     * Can be parallel
     */
    const int rowCount = row->count;
    const int *rowIndex = &row->index[0];
    const double *rowArray = &row->array[0];
    for (int i = 0; i < rowCount; i++) {
        const int index = rowIndex[i];
        const double value = rowArray[index];
        packIndex[packCount] = index + offset;
        packValue[packCount++] = value;
    }
}
void HDualRow::choose_possible() {
    /**
     * Will determine the possible variables
     * Can be parallel.
     */
    const double Ta = workModel->countUpdate < 10 ? 1e-9 :
                      workModel->countUpdate < 20 ? 3e-8 : 1e-6;
    const double Td = workModel->dblOption[DBLOPT_DUAL_TOL];
    const int sourceOut = workDelta < 0 ? -1 : 1;
    workTheta = HSOL_CONST_INF;
    workCount = 0;
    for (int i = 0; i < packCount; i++) {
        const int iCol = packIndex[i];
        const int move = workMove[iCol];
        const double alpha = packValue[i] * sourceOut * move;
        if (alpha > Ta) {
            workData[workCount++] = make_pair(iCol, alpha);
            const double relax = workDual[iCol] * move + Td;
            if (workTheta * alpha > relax)
                workTheta = relax / alpha;
        }
    }
}
void HDualRow::choose_joinpack(const HDualRow* otherRow) {
    /**
     * Will join the possible pack
     * Must be sequentail
     */
    const int otherCount = otherRow->workCount;
    const pair<int, double> *otherData = &otherRow->workData[0];
    copy(otherData, otherData + otherCount, &workData[workCount]);
    workCount = workCount + otherCount;
    workTheta = min(workTheta, otherRow->workTheta);
}
void HDualRow::choose_final() {
    /**
     * This routine choose the dual entering variable by
     * BFRT and EXPAND. (In sequential)
     *
     * It will
     * (1) reduce the candidates as a small collection
     * (2) choose by BFRT by going over break points
     * (3) choose final by alpha
     * (4) determine final flip variables
     */
    // 1. Reduce by large step BFRT
    int fullCount = workCount;
    workCount = 0;
    double totalChange = 0;
    double totalDelta = fabs(workDelta);
    double selectTheta = 10 * workTheta + 1e-7;
    for (;;) {
        for (int i = workCount; i < fullCount; i++) {
            int iCol = workData[i].first;
            double alpha = workData[i].second;
            double tight = workMove[iCol] * workDual[iCol];
            if (alpha * selectTheta >= tight) {
                swap(workData[workCount++], workData[i]);
                totalChange += workRange[iCol] * alpha;
            }
        }
        selectTheta *= 10;
        if (totalChange >= totalDelta || workCount == fullCount)
            break;
    }
    // 2. Choose by small step BFRT
    const double Td = workModel->dblOption[DBLOPT_DUAL_TOL];
    fullCount = workCount;
    workCount = 0;
    totalChange = 1e-12;
    selectTheta = workTheta;
    workGroup.clear();
    workGroup.push_back(0);
    while (selectTheta < 1e18) {
        double remainTheta = 1e100;
        for (int i = workCount; i < fullCount; i++) {
            int iCol = workData[i].first;
            double value = workData[i].second;
            double dual = workMove[iCol] * workDual[iCol];
            // Tight satisfy
            if (dual <= selectTheta * value) {
                swap(workData[workCount++], workData[i]);
                totalChange += value * (workRange[iCol]);
            } else if (dual + Td < remainTheta * value) {
                remainTheta = (dual + Td) / value;
            }
        }
        workGroup.push_back(workCount);
        selectTheta = remainTheta;
        if (totalChange >= totalDelta || workCount == fullCount)
            break;
    }
    // 3. Choose large alpha
    double finalCompare = 0;
    for (int i = 0; i < workCount; i++)
        finalCompare = max(finalCompare, workData[i].second);
    finalCompare = min(0.1 * finalCompare, 1.0);
    int countGroup = workGroup.size() - 1;
    int breakGroup = -1;
    int breakIndex = -1;
    for (int iGroup = countGroup - 1; iGroup >= 0; iGroup--) {
        double dMaxFinal = 0;
        int iMaxFinal = -1;
        for (int i = workGroup[iGroup]; i < workGroup[iGroup + 1]; i++) {
            if (dMaxFinal < workData[i].second) {
                dMaxFinal = workData[i].second;
                iMaxFinal = i;
            } else if (dMaxFinal == workData[i].second) {
                int jCol = workData[iMaxFinal].first;
                int iCol = workData[i].first;
                if (workRand[iCol] < workRand[jCol]) {
                    iMaxFinal = i;
                }
            }
        }
        if (workData[iMaxFinal].second > finalCompare) {
            breakIndex = iMaxFinal;
            breakGroup = iGroup;
            break;
        }
    }
    int sourceOut = workDelta < 0 ? -1 : 1;
    workPivot = workData[breakIndex].first;
    workAlpha = workData[breakIndex].second * sourceOut * workMove[workPivot];
    if (workDual[workPivot] * workMove[workPivot] > 0)
        workTheta = workDual[workPivot] / workAlpha;
    else
        workTheta = 0;
    // 4. Determine BFRT flip index: flip all
    fullCount = breakIndex;
    workCount = 0;
    for (int i = 0; i < workGroup[breakGroup]; i++) {
        const int iCol = workData[i].first;
        const int move = workMove[iCol];
        workData[workCount++] = make_pair(iCol, move * workRange[iCol]);
    }
    if (workTheta == 0)
        workCount = 0;
    sort(workData.begin(), workData.begin() + workCount);
}
void HDualRow::update_flip(HVector *bfrtColumn) {
    bfrtColumn->clear();
    for (int i = 0; i < workCount; i++) {
        const int iCol = workData[i].first;
        const double change = workData[i].second;
        workModel->flipBound(iCol);
        workModel->getMatrix()->collect_aj(*bfrtColumn, iCol, change);
    }
}
void HDualRow::update_dual(double theta) {
    double *workDual = workModel->getWorkDual();
    for (int i = 0; i < packCount; i++)
        workDual[packIndex[i]] -= theta * packValue[i];
}
void HDualRow::create_Freelist() {
    freeList.clear();
    const int *nonbasicFlag = workModel->getNonbasicFlag();
    for (int i = 0; i < workModel->getNumTot(); i++) {
        if (nonbasicFlag[i] && workRange[i] > 1.5 * HSOL_CONST_INF)
            freeList.insert(i);
    }
}
void HDualRow::create_Freemove(HVector *row_ep) {
    if (!freeList.empty()) {
        double Ta = workModel->countUpdate < 10 ? 1e-9 :
                    workModel->countUpdate < 20 ? 3e-8 : 1e-6;
        int sourceOut = workDelta < 0 ? -1 : 1;
        set<int>::iterator sit;
        for (sit = freeList.begin(); sit != freeList.end(); sit++) {
            int iCol = *sit;
            assert(iCol < workModel->getNumCol());
            double alpha = workModel->getMatrix()->compute_dot(*row_ep, iCol);
            if (fabs(alpha) > Ta) {
                if (alpha * sourceOut > 0)
                    workModel->getNonbasicMove()[iCol] = 1;
                else
                    workModel->getNonbasicMove()[iCol] = -1;
            }
        }
    }
}
void HDualRow::delete_Freemove() {
    if (!freeList.empty()) {
        set<int>::iterator sit;
        for (sit = freeList.begin(); sit != freeList.end(); sit++) {
            int iCol = *sit;
            assert(iCol < workModel->getNumCol());
            workModel->getNonbasicMove()[iCol] = 0;
        }
    }
}
void HDualRow::delete_Freelist(int iColumn) {
    if (!freeList.empty()) {
        if (freeList.count(iColumn))
            freeList.erase(iColumn);
    }
}
// #include <cmath>
// #include <cassert>
// #include <iostream>
// #include <stdexcept>
using namespace std;
void HFactor::copyFrom(const HFactor *from) {
    numRow = from->numRow;
    numCol = from->numCol;
    Astart = from->Astart;
    Aindex = from->Aindex;
    Avalue = from->Avalue;
    baseIndex = from->baseIndex;
    updateMethod = from->updateMethod;
    nwork = from->nwork;
    iwork = from->iwork;
    dwork = from->dwork;
    LpivotLookup = from->LpivotLookup;
    LpivotIndex = from->LpivotIndex;
    Lstart = from->Lstart;
    Lindex = from->Lindex;
    Lvalue = from->Lvalue;
    LRstart = from->LRstart;
    LRindex = from->LRindex;
    LRvalue = from->LRvalue;
    UpivotLookup = from->UpivotLookup;
    UpivotIndex = from->UpivotIndex;
    UpivotValue = from->UpivotValue;
    UmeritX = from->UmeritX;
    UtotalX = from->UtotalX;
    Ustart = from->Ustart;
    Ulastp = from->Ulastp;
    Uindex = from->Uindex;
    Uvalue = from->Uvalue;
    URstart = from->URstart;
    URlastp = from->URlastp;
    URspace = from->URspace;
    URindex = from->URindex;
    URvalue = from->URvalue;
    PFpivotValue = from->PFpivotValue;
    PFpivotIndex = from->PFpivotIndex;
    PFstart = from->PFstart;
    PFindex = from->PFindex;
    PFvalue = from->PFvalue;
}
void solveMatrixT(const int Xstart, const int Xend, const int Ystart,
        const int Yend, const int *Tindex, const double *Tvalue,
        const double Tpivot, int *RHScount, int *RHSindex, double *RHSarray) {
    // Collect by X
    double pivotX = 0;
    for (int k = Xstart; k < Xend; k++)
        pivotX += Tvalue[k] * RHSarray[Tindex[k]];
    // Scatter by Y
    if (fabs(pivotX) > HSOL_CONST_TINY) {
        int workCount = *RHScount;
        pivotX /= Tpivot;
        for (int k = Ystart; k < Yend; k++) {
            const int index = Tindex[k];
            const double value0 = RHSarray[index];
            const double value1 = value0 - pivotX * Tvalue[k];
            if (value0 == 0)
                RHSindex[workCount++] = index;
            RHSarray[index] =
                    (fabs(value1) < HSOL_CONST_TINY) ? HSOL_CONST_ZERO : value1;
        }
        *RHScount = workCount;
    }
}
void solveHyper(const int Hsize, const int *Hlookup, const int *HpivotIndex,
        const double *HpivotValue, const int *Hstart, const int *Hend,
        const int *Hindex, const double *Hvalue, HVector *rhs) {
    int RHS_Tick = rhs->pseudoTick;
    int RHScount = rhs->count;
    int *RHSindex = &rhs->index[0];
    double *RHSarray = &rhs->array[0];
    // Take count
    // Build list
    char *listMark = &rhs->cwork[0];
    int *listIndex = &rhs->iwork[0];
    int *listStack = &rhs->iwork[Hsize];
    int listCount = 0;
    int countPivot = 0;
    int countEntry = 0;
    for (int i = 0; i < RHScount; i++) {
        // Skip touched index
        int iTrans = Hlookup[RHSindex[i]]; // XXX: this contains a bug iTran
        if (listMark[iTrans]) // XXX bug here
            continue;
        int Hi = iTrans; // H matrix pivot index
        int Hk = Hstart[Hi]; // H matrix non zero position
        int nStack = -1; // Usage of the stack (-1 not used)
        listMark[Hi] = 1; // Mark this as touched
        for (;;) {
            if (Hk < Hend[Hi]) {
                int Hi_sub = Hlookup[Hindex[Hk++]];
                if (listMark[Hi_sub] == 0) { // Go to a child
                    listMark[Hi_sub] = 1; // Mark as touched
                    listStack[++nStack] = Hi; // Store current into stack
                    listStack[++nStack] = Hk;
                    Hi = Hi_sub; // Replace current with child
                    Hk = Hstart[Hi];
                    if (Hi >= Hsize) {
                        countPivot++;
                        countEntry += Hend[Hi] - Hstart[Hi];
                        RHS_Tick += 10;
                    }
                }
            } else {
                listIndex[listCount++] = Hi;
                if (nStack == -1) // Quit on empty stack
                    break;
                Hk = listStack[nStack--]; // Back to last in stack
                Hi = listStack[nStack--];
            }
        }
    }
    rhs->fakeTick += countPivot * 20 + countEntry * 10;
    // Solve with list
    if (HpivotValue == 0) {
        RHScount = 0;
        for (int iList = listCount - 1; iList >= 0; iList--) {
            int i = listIndex[iList];
            listMark[i] = 0;
            int pivotRow = HpivotIndex[i];
            double pivotX = RHSarray[pivotRow];
            if (fabs(pivotX) > HSOL_CONST_TINY) {
                RHSindex[RHScount++] = pivotRow;
                const int start = Hstart[i];
                const int end = Hend[i];
                RHS_Tick += end - start;
                for (int k = start; k < end; k++)
                    RHSarray[Hindex[k]] -= pivotX * Hvalue[k];
            } else
                RHSarray[pivotRow] = 0;
        }
        rhs->pseudoTick = RHS_Tick;
        rhs->count = RHScount;
    } else {
        RHScount = 0;
        for (int iList = listCount - 1; iList >= 0; iList--) {
            int i = listIndex[iList];
            listMark[i] = 0;
            int pivotRow = HpivotIndex[i];
            double pivotX = RHSarray[pivotRow];
            if (fabs(pivotX) > HSOL_CONST_TINY) {
                pivotX /= HpivotValue[i];
                RHSarray[pivotRow] = pivotX;
                RHSindex[RHScount++] = pivotRow;
                const int start = Hstart[i];
                const int end = Hend[i];
                RHS_Tick += end - start;
                for (int k = start; k < end; k++)
                    RHSarray[Hindex[k]] -= pivotX * Hvalue[k];
            } else
                RHSarray[pivotRow] = 0;
        }
        rhs->pseudoTick = RHS_Tick;
        rhs->count = RHScount;
    }
}
void HFactor::setup(int numCol_, int numRow_, int *Astart_, int *Aindex_,
        double *Avalue_, int *baseIndex_, int updateMethod_) {
    // Copy Problem size and (pointer to) coefficient matrix
    numRow = numRow_;
    numCol = numCol_;
    Astart = Astart_;
    Aindex = Aindex_;
    Avalue = Avalue_;
    baseIndex = baseIndex_;
    updateMethod = updateMethod_;
    // Allocate for working buffer
    iwork.reserve(numRow * 2);
    dwork.assign(numRow, 0);
    // Find Basis matrix limit size
    int BlimitX = 0;
    iwork.assign(numRow + 1, 0);
    for (int i = 0; i < numCol; i++)
        iwork[Astart[i + 1] - Astart[i]]++;
    for (int i = numRow, counted = 0; i >= 0 && counted < numRow; i--)
        BlimitX += i * iwork[i], counted += iwork[i];
    BlimitX += numRow;
    // Allocate spaces for basis matrix, L, U factor and Update buffer
    Bstart.resize(numRow + 1, 0);
    Bindex.resize(BlimitX);
    Bvalue.resize(BlimitX);
    // Allocate spaces for pivot records
    permute.resize(numRow);
    // Allocate spaces for Markowitz matrices
    MCstart.resize(numRow);
    MCcountA.resize(numRow);
    MCcountN.resize(numRow);
    MCspace.resize(numRow);
    MCminpivot.resize(numRow);
    MCindex.resize(BlimitX * 2);
    MCvalue.resize(BlimitX * 2);
    MRstart.resize(numRow);
    MRcount.resize(numRow);
    MRspace.resize(numRow);
    MRcountb4.resize(numRow);
    MRindex.resize(BlimitX * 2);
    McolumnMark.assign(numRow, 0);
    McolumnIndex.resize(numRow);
    McolumnArray.assign(numRow, 0);
    // Allocate spaces for count-link-list
    clinkFirst.assign(numRow + 1, -1);
    clinkNext.resize(numRow);
    clinkLast.resize(numRow);
    rlinkFirst.assign(numRow + 1, -1);
    rlinkNext.resize(numRow);
    rlinkLast.resize(numRow);
    // Allocate spaces for L factor
    LpivotLookup.resize(numRow);
    LpivotIndex.reserve(numRow);
    Lstart.reserve(numRow + 1);
    Lindex.reserve(BlimitX * 3);
    Lvalue.reserve(BlimitX * 3);
    LRstart.reserve(numRow + 1);
    LRindex.reserve(BlimitX * 3);
    LRvalue.reserve(BlimitX * 3);
    // Allocate spaces for U factor
    UpivotLookup.resize(numRow);
    UpivotIndex.reserve(numRow + 1000);
    UpivotValue.reserve(numRow + 1000);
    Ustart.reserve(numRow + 1000 + 1);
    Ulastp.reserve(numRow + 1000);
    Uindex.reserve(BlimitX * 3);
    Uvalue.reserve(BlimitX * 3);
    URstart.reserve(numRow + 1000 + 1);
    URlastp.reserve(numRow + 1000);
    URspace.reserve(numRow + 1000);
    URindex.reserve(BlimitX * 3);
    URvalue.reserve(BlimitX * 3);
    // Allocate spaces for Update buffer
    PFpivotValue.reserve(1000);
    PFpivotIndex.reserve(1000);
    PFstart.reserve(2000 + 1);
    PFindex.reserve(BlimitX * 4);
    PFvalue.reserve(BlimitX * 4);
}
void HFactor::change(int updateMethod_) {
    updateMethod = updateMethod_;
}
void HFactor::build() {
    pseudoTick = 0;
    // Build the L, U factor
    fakeTick = 0;
    buildSimple();
    buildKernel();
    buildFinish();
    // I can't believe I made this bug!!!
    pseudoTick *= 8;
//    printf("realTick = %10.0f, COMPARE = %10.4f  COMPAREX = %10.4f\n", realTick,
//            (realTick) / (fakeTick), pseudoTick / fakeTick);
}
void HFactor::ftran(HVector& vector, double hist_dsty) const {
    ftranL(vector, hist_dsty);
    ftranU(vector, hist_dsty);
}
void HFactor::btran(HVector& vector, double hist_dsty) const {
    btranU(vector, hist_dsty);
    btranL(vector, hist_dsty);
}
void HFactor::update(HVector *aq, HVector *ep, int *iRow, int *hint) {
    // Special case
    if (aq->next) {
        updateCFT(aq, ep, iRow, hint);
        return;
    }
    if (updateMethod == UPDATE_METHOD_FT)
        updateFT(aq, ep, *iRow, hint);
    if (updateMethod == UPDATE_METHOD_PF)
        updatePF(aq, ep, *iRow, hint);
    if (updateMethod == UPDATE_METHOD_MPF)
        updateMPF(aq, ep, *iRow, hint);
    if (updateMethod == UPDATE_METHOD_APF)
        updateAPF(aq, ep, *iRow, hint);
}
void HFactor::buildSimple() {
    /**
     * 0. Clear L and U factor
     */
    Lstart.clear();
    Lstart.push_back(0);
    Lindex.clear();
    Lvalue.clear();
    UpivotIndex.clear();
    UpivotValue.clear();
    Ustart.clear();
    Ustart.push_back(0);
    Uindex.clear();
    Uvalue.clear();
    /**
     * 1. Prepare basis matrix and deal with unit columns
     */
    int BcountX = 0;
    fill_n(&MRcountb4[0], numRow, 0);
    nwork = 0;
    for (int iCol = 0; iCol < numRow; iCol++) {
      int iMat = baseIndex[iCol];
      int iRow = -1;
      if (iMat >= numCol) {
 // 1.1 Logical column
 // Check for double pivot
 int lc_iRow = iMat - numCol;
 if (MRcountb4[lc_iRow] >= 0) {
   iRow = lc_iRow;
 } else {
   printf("STRANGE: Found a logical column with pivot already in row %d\n", lc_iRow);
   MRcountb4[lc_iRow]++;
   Bindex[BcountX] = lc_iRow;
   Bvalue[BcountX++] = 1.0;
   iwork[nwork++] = iCol;
 }
      } else {
 // 1.2 Structural column
 int start = Astart[iMat];
 int count = Astart[iMat + 1] - start;
 int lc_iRow = Aindex[start];
 // Check for unit column with double pivot
 bool unit_column = count == 1 && Avalue[start] == 1;
 if (unit_column && MRcountb4[lc_iRow] >= 0) {
   iRow = lc_iRow;
 } else {
   if (unit_column) printf("STRANGE: Found a second unit column with pivot in row %d\n", lc_iRow);
   for (int k = start; k < start + count; k++) {
     MRcountb4[Aindex[k]]++;
     Bindex[BcountX] = Aindex[k];
     Bvalue[BcountX++] = Avalue[k];
   }
   iwork[nwork++] = iCol;
 }
      }
      if (iRow >= 0) {
 // 1.3 Record unit column
 //printf("Found unit column (Row, Col) = (%d, %d)\n", Lindex.size(), Uindex.size());
 permute[iCol] = iRow;
 Lstart.push_back(Lindex.size());
 UpivotIndex.push_back(iRow);
 UpivotValue.push_back(1);
 Ustart.push_back(Uindex.size());
 MRcountb4[iRow] = -numRow;
 // printf("Found unit column (Row, Col) = (%d, %d)\n", iRow, iCol);
      }
      Bstart[iCol + 1] = BcountX;
    }
    BtotalX = numRow - nwork + BcountX;
    pseudoTick += (numRow - nwork) * 2;
    pseudoTick += BcountX * 1.5;
//    count1 = 0;
    // Comments: for pds-20, dfl001: 60 / 80
    // Comments: when system is large: enlarge
    // Comments: when system is small: decrease
    fakeTick += BcountX * 60 + (numRow - nwork) * 80;
    /**
     * 2. Search for and deal with singletons
     */
    double t2_search = 0;
    double t2_storeL = Lindex.size();
    double t2_storeU = Uindex.size();
    double t2_storep = nwork;
    while (nwork > 0) {
        int nworkLast = nwork;
        nwork = 0;
        for (int i = 0; i < nworkLast; i++) {
            const int iCol = iwork[i];
            const int start = Bstart[iCol];
            const int end = Bstart[iCol + 1];
            int pivot_k = -1;
            int found_row_singleton = 0;
            int count = 0;
            // 2.1 Search for singleton
            t2_search += end - start;
            pseudoTick += (end - start) * 0.5;
            for (int k = start; k < end; k++) {
                const int iRow = Bindex[k];
                if (MRcountb4[iRow] == 1) {
                    pivot_k = k;
                    found_row_singleton = 1;
                    break;
                }
                if (MRcountb4[iRow] > 1) {
                    pivot_k = k;
                    count++;
                }
            }
            if (found_row_singleton) {
                // 2.2 Deal with row singleton
                const double pivotX = 1 / Bvalue[pivot_k];
                for (int section = 0; section < 2; section++) {
                    int p0 = section == 0 ? start : pivot_k + 1;
                    int p1 = section == 0 ? pivot_k : end;
                    for (int k = p0; k < p1; k++) {
                        int iRow = Bindex[k];
                        if (MRcountb4[iRow] > 0) {
                            Lindex.push_back(iRow);
                            Lvalue.push_back(Bvalue[k] * pivotX);
                        } else {
                            Uindex.push_back(iRow);
                            Uvalue.push_back(Bvalue[k]);
                        }
                        MRcountb4[iRow]--;
                    }
                }
                int iRow = Bindex[pivot_k];
                MRcountb4[iRow] = 0;
                permute[iCol] = iRow;
                Lstart.push_back(Lindex.size());
                UpivotIndex.push_back(iRow);
                UpivotValue.push_back(Bvalue[pivot_k]);
                Ustart.push_back(Uindex.size());
            } else if (count == 1) {
                // 2.3 Deal with column singleton
                for (int k = start; k < pivot_k; k++) {
                    Uindex.push_back(Bindex[k]);
                    Uvalue.push_back(Bvalue[k]);
                }
                for (int k = pivot_k + 1; k < end; k++) {
                    Uindex.push_back(Bindex[k]);
                    Uvalue.push_back(Bvalue[k]);
                }
                int iRow = Bindex[pivot_k];
                MRcountb4[iRow] = 0;
                permute[iCol] = iRow;
                Lstart.push_back(Lindex.size());
                UpivotIndex.push_back(iRow);
                UpivotValue.push_back(Bvalue[pivot_k]);
                Ustart.push_back(Uindex.size());
            } else {
                iwork[nwork++] = iCol;
            }
        }
        // No singleton found in the last pass
        pseudoTick += (nworkLast - nwork) * 2; // For pivot
        if (nworkLast == nwork)
            break;
    }
    t2_storeL = Lindex.size() - t2_storeL;
    t2_storeU = Uindex.size() - t2_storeU;
    t2_storep = t2_storep - nwork;
    fakeTick += t2_search * 20 + (t2_storep + t2_storeL + t2_storeU) * 80;
    pseudoTick += (Lindex.size() + Uindex.size()) * 1.5; // For factor
    /**
     * 3. Prepare the kernel parts
     */
    // 3.1 Prepare row links, row matrix spaces
    rlinkFirst.assign(numRow + 1, -1);
    MRcount.assign(numRow, 0);
    int MRcountX = 0;
    for (int iRow = 0; iRow < numRow; iRow++) {
        int count = MRcountb4[iRow];
        if (count > 0) {
            MRstart[iRow] = MRcountX;
            MRspace[iRow] = count * 2;
            MRcountX += count * 2;
            rlinkAdd(iRow, count);
        }
    }
    MRindex.resize(MRcountX);
    // 3.2 Prepare column links, kernel matrix
    clinkFirst.assign(numRow + 1, -1);
    MCindex.clear();
    MCvalue.clear();
    MCcountA.assign(numRow, 0);
    MCcountN.assign(numRow, 0);
    int MCcountX = 0;
    for (int i = 0; i < nwork; i++) {
        int iCol = iwork[i];
        MCstart[iCol] = MCcountX;
        MCspace[iCol] = (Bstart[iCol + 1] - Bstart[iCol]) * 2;
        MCcountX += MCspace[iCol];
        MCindex.resize(MCcountX);
        MCvalue.resize(MCcountX);
        for (int k = Bstart[iCol]; k < Bstart[iCol + 1]; k++) {
            const int iRow = Bindex[k];
            const double value = Bvalue[k];
            if (MRcountb4[iRow] > 0) {
                colInsert(iCol, iRow, value);
                rowInsert(iCol, iRow);
            } else {
                colStoreN(iCol, iRow, value);
            }
        }
        colFixMax(iCol);
        clinkAdd(iCol, MCcountA[iCol]);
    }
    pseudoTick += nwork * 2; // DLL
    pseudoTick += MCcountX; // MATRIX
    fakeTick += (numRow + nwork + MCcountX) * 40 + MRcountX * 20;
}
void HFactor::buildKernel() {
    // Deal with the kernel part by 'n-work' pivoting
    double fake_search = 0;
    double fake_fill = 0;
    double fake_eliminate = 0;
    while (nwork-- > 0) {
        /**
         * 1. Search for the pivot
         */
        int jColPivot = -1;
        int iRowPivot = -1;
        // 1.1. Setup search merits
        int searchLimit = min(nwork, 8);
        int searchCount = 0;
        double meritLimit = 1.0 * numRow * numRow;
        double meritPivot = meritLimit;
        // 1.2. Search for local singletons
        bool foundPivot = false;
        if (!foundPivot && clinkFirst[1] != -1) {
            jColPivot = clinkFirst[1];
            iRowPivot = MCindex[MCstart[jColPivot]];
            foundPivot = true;
        }
        if (!foundPivot && rlinkFirst[1] != -1) {
            iRowPivot = rlinkFirst[1];
            jColPivot = MRindex[MRstart[iRowPivot]];
            foundPivot = true;
        }
        // 1.3. Major search loop
        for (int count = 2; !foundPivot && count <= numRow; count++) {
            // 1.3.1 Search for columns
            for (int j = clinkFirst[count]; j != -1; j = clinkNext[j]) {
                double minpivot = MCminpivot[j];
                int start = MCstart[j];
                int end = start + MCcountA[j];
                pseudoTick += MCcountA[j] * 0.3;
                for (int k = start; k < end; k++) {
                    if (fabs(MCvalue[k]) >= minpivot) {
                        int i = MCindex[k];
                        int rowCount = MRcount[i];
                        double meritLocal = 1.0 * (count - 1) * (rowCount - 1);
                        if (meritPivot > meritLocal) {
                            meritPivot = meritLocal;
                            jColPivot = j;
                            iRowPivot = i;
                            foundPivot = foundPivot || (rowCount < count);
                        }
                    }
                }
                if (searchCount++ >= searchLimit && meritPivot < meritLimit)
                    foundPivot = true;
                if (foundPivot)
                    break;
                fake_search += count;
            }
            // 1.3.2 Search for rows
            for (int i = rlinkFirst[count]; i != -1; i = rlinkNext[i]) {
                int start = MRstart[i];
                int end = start + MRcount[i];
                pseudoTick += MRcount[i] * 0.3;
                for (int k = start; k < end; k++) {
                    int j = MRindex[k];
                    int columnCount = MCcountA[j];
                    double meritLocal = 1.0 * (count - 1) * (columnCount - 1);
                    if (meritLocal < meritPivot) {
                        int ifind = MCstart[j];
                        while (MCindex[ifind] != i)
                            ifind++;
                        pseudoTick += columnCount * 0.05;
                        if (fabs(MCvalue[ifind]) >= MCminpivot[j]) {
                            meritPivot = meritLocal;
                            jColPivot = j;
                            iRowPivot = i;
                            foundPivot = foundPivot || (columnCount <= count);
                        }
                    }
                }
                if (searchCount++ >= searchLimit && meritPivot < meritLimit)
                    foundPivot = true;
                if (foundPivot)
                    break;
            }
            fake_search += count;
        }
        // 1.4. If we found nothing: tell singular
        if (!foundPivot)
            throw runtime_error("singular-basis-matrix");
        /**
         * 2. Elimination other elements by the pivot
         */
        // XXX NEED TO UPDATE THIS
        pseudoTick += MCcountA[jColPivot] * 2 + MCcountN[jColPivot];
        // 2.1. Delete the pivot
        double pivotX = colDelete(jColPivot, iRowPivot);
        rowDelete(jColPivot, iRowPivot);
        clinkDel(jColPivot);
        rlinkDel(iRowPivot);
        permute[jColPivot] = iRowPivot;
        // 2.2. Store active pivot column to L
        int start_A = MCstart[jColPivot];
        int end_A = start_A + MCcountA[jColPivot];
        int McolumnCount = 0;
        for (int k = start_A; k < end_A; k++) {
            const int iRow = MCindex[k];
            const double value = MCvalue[k] / pivotX;
            McolumnIndex[McolumnCount++] = iRow;
            McolumnArray[iRow] = value;
            McolumnMark[iRow] = 1;
            Lindex.push_back(iRow);
            Lvalue.push_back(value);
            MRcountb4[iRow] = MRcount[iRow];
            rowDelete(jColPivot, iRow);
        }
        Lstart.push_back(Lindex.size());
        fake_fill += 2 * MCcountA[jColPivot];
        // 2.3. Store non active pivot column to U
        int end_N = start_A + MCspace[jColPivot];
        int start_N = end_N - MCcountN[jColPivot];
        for (int i = start_N; i < end_N; i++) {
            Uindex.push_back(MCindex[i]);
            Uvalue.push_back(MCvalue[i]);
        }
        UpivotIndex.push_back(iRowPivot);
        UpivotValue.push_back(pivotX);
        Ustart.push_back(Uindex.size());
        fake_fill += end_N - start_N;
        // 2.4. Loop over pivot row to eliminate other column
        const int row_start = MRstart[iRowPivot];
        const int row_end = row_start + MRcount[iRowPivot];
        for (int row_k = row_start; row_k < row_end; row_k++) {
            // 2.4.1. My pointer
            int iCol = MRindex[row_k];
            const int my_count = MCcountA[iCol];
            const int my_start = MCstart[iCol];
            const int my_end = my_start + my_count - 1;
            double my_pivot = colDelete(iCol, iRowPivot);
            colStoreN(iCol, iRowPivot, my_pivot);
            // 2.4.2. Elimination on the overlapping part
            int nFillin = McolumnCount;
            int nCancel = 0;
            pseudoTick += McolumnCount;
            for (int my_k = my_start; my_k < my_end; my_k++) {
                int iRow = MCindex[my_k];
                double value = MCvalue[my_k];
                if (McolumnMark[iRow]) {
                    McolumnMark[iRow] = 0;
                    nFillin--;
                    value -= my_pivot * McolumnArray[iRow];
                    if (fabs(value) < HSOL_CONST_TINY) {
                        value = 0;
                        nCancel++;
                    }
                    MCvalue[my_k] = value;
                }
            }
            fake_eliminate += McolumnCount;
            fake_eliminate += nFillin * 2;
            // 2.4.3. Remove cancellation gaps
            if (nCancel > 0) {
                int new_end = my_start;
                for (int my_k = my_start; my_k < my_end; my_k++) {
                    if (MCvalue[my_k] != 0) {
                        MCindex[new_end] = MCindex[my_k];
                        MCvalue[new_end++] = MCvalue[my_k];
                    } else {
                        rowDelete(iCol, MCindex[my_k]);
                    }
                }
                MCcountA[iCol] = new_end - my_start;
            }
            // 2.4.4. Insert fill-in
            if (nFillin > 0) {
                pseudoTick += nFillin * 5;
                // 2.4.4.1 Check column size
                if (MCcountA[iCol] + MCcountN[iCol] + nFillin > MCspace[iCol]) {
                    // p1&2=active, p3&4=non active, p5=new p1, p7=new p3
                    int p1 = MCstart[iCol];
                    int p2 = p1 + MCcountA[iCol];
                    int p3 = p1 + MCspace[iCol] - MCcountN[iCol];
                    int p4 = p1 + MCspace[iCol];
                    MCspace[iCol] += max(MCspace[iCol], nFillin);
                    int p5 = MCstart[iCol] = MCindex.size();
                    int p7 = p5 + MCspace[iCol] - MCcountN[iCol];
                    MCindex.resize(p5 + MCspace[iCol]);
                    MCvalue.resize(p5 + MCspace[iCol]);
                    copy(&MCindex[p1], &MCindex[p2], &MCindex[p5]);
                    copy(&MCvalue[p1], &MCvalue[p2], &MCvalue[p5]);
                    copy(&MCindex[p3], &MCindex[p4], &MCindex[p7]);
                    copy(&MCvalue[p3], &MCvalue[p4], &MCvalue[p7]);
                }
                // 2.4.4.2 Fill into column copy
                for (int i = 0; i < McolumnCount; i++) {
                    int iRow = McolumnIndex[i];
                    if (McolumnMark[iRow])
                        colInsert(iCol, iRow, -my_pivot * McolumnArray[iRow]);
                }
                // 2.4.4.3 Fill into the row copy
                for (int i = 0; i < McolumnCount; i++) {
                    int iRow = McolumnIndex[i];
                    if (McolumnMark[iRow]) {
                        // Expand row space
                        if (MRcount[iRow] == MRspace[iRow]) {
                            int p1 = MRstart[iRow];
                            int p2 = p1 + MRcount[iRow];
                            int p3 = MRstart[iRow] = MRindex.size();
                            MRspace[iRow] *= 2;
                            MRindex.resize(p3 + MRspace[iRow]);
                            copy(&MRindex[p1], &MRindex[p2], &MRindex[p3]);
                        }
                        rowInsert(iCol, iRow);
                    }
                }
            }
            // 2.4.5. Reset pivot column mark
            for (int i = 0; i < McolumnCount; i++)
                McolumnMark[McolumnIndex[i]] = 1;
            // 2.4.6. Fix max value and link list
            colFixMax(iCol);
            if (my_count != MCcountA[iCol]) {
                clinkDel(iCol);
                clinkAdd(iCol, MCcountA[iCol]);
            }
        }
        // 2.5. Clear pivot column buffer
        for (int i = 0; i < McolumnCount; i++)
            McolumnMark[McolumnIndex[i]] = 0;
        // 2.6. Correct row links for the remain active part
        for (int i = start_A; i < end_A; i++) {
            int iRow = MCindex[i];
            if (MRcountb4[iRow] != MRcount[iRow]) {
                rlinkDel(iRow);
                rlinkAdd(iRow, MRcount[iRow]);
            }
        }
    }
    fakeTick += fake_search * 20 + fake_fill * 160 + fake_eliminate * 80;
}
void HFactor::buildFinish() {
    // The look up table
    for (int i = 0; i < numRow; i++)
        UpivotLookup[UpivotIndex[i]] = i;
    LpivotIndex = UpivotIndex;
    LpivotLookup = UpivotLookup;
    // LR space
    int LcountX = Lindex.size();
    LRindex.resize(LcountX);
    LRvalue.resize(LcountX);
    // LR pointer
    iwork.assign(numRow, 0);
    for (int k = 0; k < LcountX; k++)
        iwork[LpivotLookup[Lindex[k]]]++;
    LRstart.assign(numRow + 1, 0);
    for (int i = 1; i <= numRow; i++)
        LRstart[i] = LRstart[i - 1] + iwork[i - 1];
    // LR elements
    iwork.assign(&LRstart[0], &LRstart[numRow]);
    for (int i = 0; i < numRow; i++) {
        const int index = LpivotIndex[i];
        for (int k = Lstart[i]; k < Lstart[i + 1]; k++) {
            int iRow = LpivotLookup[Lindex[k]];
            int iPut = iwork[iRow]++;
            LRindex[iPut] = index;
            LRvalue[iPut] = Lvalue[k];
        }
    }
    // U pointer
    Ulastp.assign(&Ustart[1], &Ustart[numRow + 1]);
    Ustart.resize(numRow);
    // UR space
    int UcountX = Uindex.size();
    int URstuffX = updateMethod == UPDATE_METHOD_FT ? 5 : 0;
    int URcountX = UcountX + URstuffX * numRow;
    URindex.resize(URcountX);
    URvalue.resize(URcountX);
    // UR pointer
    URstart.assign(numRow + 1, 0);
    URlastp.assign(numRow, 0);
    URspace.assign(numRow, URstuffX);
    for (int k = 0; k < UcountX; k++)
        URlastp[UpivotLookup[Uindex[k]]]++;
    for (int i = 1; i <= numRow; i++)
        URstart[i] = URstart[i - 1] + URlastp[i - 1] + URstuffX;
    URstart.resize(numRow);
    // UR element
    URlastp = URstart;
    for (int i = 0; i < numRow; i++) {
        const int index = UpivotIndex[i];
        for (int k = Ustart[i]; k < Ulastp[i]; k++) {
            int iRow = UpivotLookup[Uindex[k]];
            int iPut = URlastp[iRow]++;
            URindex[iPut] = index;
            URvalue[iPut] = Uvalue[k];
        }
    }
    // Re-factor merit
    UmeritX = numRow + (LcountX + UcountX) * 1.5;
    UtotalX = UcountX;
    if (updateMethod == UPDATE_METHOD_PF)
        UmeritX = numRow + UcountX * 4;
    if (updateMethod == UPDATE_METHOD_MPF)
        UmeritX = numRow + UcountX * 3;
    // Clear update buffer
    PFpivotValue.clear();
    PFpivotIndex.clear();
    PFstart.clear();
    PFstart.push_back(0);
    PFindex.clear();
    PFvalue.clear();
    // Finally, permute the base index
    iwork.assign(baseIndex, baseIndex + numRow);
    for (int i = 0; i < numRow; i++)
        baseIndex[permute[i]] = iwork[i];
    pseudoTick += LcountX + UcountX + numRow;
    FtotalX = LcountX + UcountX + numRow;
    fakeTick += numRow * 80 + (LcountX + UcountX) * 60;
}
void HFactor::ftranL(HVector& rhs, double hist_dsty) const {
    const double hyperFTRANL = 0.15;
    const double hyperCANCEL = 0.05;
    if (updateMethod == UPDATE_METHOD_APF) {
        rhs.tight();
        rhs.pack();
        ftranAPF(rhs);
        rhs.tight();
    }
    double curr_dsty = 1.0 * rhs.count / numRow;
    if (curr_dsty > hyperCANCEL || hist_dsty > hyperFTRANL) {
        // Alias to RHS
        int RHScount = 0;
        int *RHSindex = &rhs.index[0];
        double *RHSarray = &rhs.array[0];
        // Alias to factor L
        const int *Lstart = &this->Lstart[0];
        const int *Lindex = &this->Lindex[0];
        const double *Lvalue = &this->Lvalue[0];
        // Transform
        for (int i = 0; i < numRow; i++) {
            int pivotRow = LpivotIndex[i];
            const double pivotX = RHSarray[pivotRow];
            if (fabs(pivotX) > HSOL_CONST_TINY) {
                RHSindex[RHScount++] = pivotRow;
                const int start = Lstart[i];
                const int end = Lstart[i + 1];
                for (int k = start; k < end; k++)
                    RHSarray[Lindex[k]] -= pivotX * Lvalue[k];
            } else
                RHSarray[pivotRow] = 0;
        }
        // Save the count
        rhs.count = RHScount;
    } else {
        solveHyper(numRow, &LpivotLookup[0], &LpivotIndex[0], 0, &Lstart[0],
                &Lstart[1], &Lindex[0], &Lvalue[0], &rhs);
    }
}
void HFactor::btranL(HVector& rhs, double hist_dsty) const {
    const double hyperBTRANL = 0.10;
    const double hyperCANCEL = 0.05;
    double curr_dsty = 1.0 * rhs.count / numRow;
    if (curr_dsty > hyperCANCEL || hist_dsty > hyperBTRANL) {
        // Alias to RHS
        int RHScount = 0;
        int *RHSindex = &rhs.index[0];
        double *RHSarray = &rhs.array[0];
        // Alias to factor L
        const int *LRstart = &this->LRstart[0];
        const int *LRindex = &this->LRindex[0];
        const double *LRvalue = &this->LRvalue[0];
        // Transform
        for (int i = numRow - 1; i >= 0; i--) {
            int pivotRow = LpivotIndex[i];
            const double pivotX = RHSarray[pivotRow];
            if (fabs(pivotX) > HSOL_CONST_TINY) {
                RHSindex[RHScount++] = pivotRow;
                RHSarray[pivotRow] = pivotX;
                const int start = LRstart[i];
                const int end = LRstart[i + 1];
                for (int k = start; k < end; k++)
                    RHSarray[LRindex[k]] -= pivotX * LRvalue[k];
            } else
                RHSarray[pivotRow] = 0;
        }
        // Save the count
        rhs.count = RHScount;
    } else {
        solveHyper(numRow, &LpivotLookup[0], &LpivotIndex[0], 0, &LRstart[0],
                &LRstart[1], &LRindex[0], &LRvalue[0], &rhs);
    }
    if (updateMethod == UPDATE_METHOD_APF) {
        btranAPF(rhs);
        rhs.tight();
        rhs.pack();
    }
}
void HFactor::ftranU(HVector& rhs, double hist_dsty) const {
    // The update part
    if (updateMethod == UPDATE_METHOD_FT) {
        ftranFT(rhs);
        rhs.tight();
        rhs.pack();
    }
    if (updateMethod == UPDATE_METHOD_MPF) {
        ftranMPF(rhs);
        rhs.tight();
        rhs.pack();
    }
    // The regular part
    const double hyperFTRANU = 0.10;
    const double hyperCANCEL = 0.05;
    double curr_dsty = 1.0 * rhs.count / numRow;
    if (curr_dsty > hyperCANCEL || hist_dsty > hyperFTRANU) {
        // Alias to non constant
        int RHS_Tick = rhs.pseudoTick;
        int RHScount = 0;
        int *RHSindex = &rhs.index[0];
        double *RHSarray = &rhs.array[0];
        // Alias to the factor
        const int *Ustart = &this->Ustart[0];
        const int *Uend = &this->Ulastp[0];
        const int *Uindex = &this->Uindex[0];
        const double *Uvalue = &this->Uvalue[0];
        // Transform
        double RHS_start = RHS_Tick;
        int UpivotCount = UpivotIndex.size();
        for (int iLogic = UpivotCount - 1; iLogic >= 0; iLogic--) {
            // Skip void
            if (UpivotIndex[iLogic] == -1)
                continue;
            // Normal part
            const int pivotRow = UpivotIndex[iLogic];
            double pivotX = RHSarray[pivotRow];
            if (fabs(pivotX) > HSOL_CONST_TINY) {
                pivotX /= UpivotValue[iLogic];
                RHSindex[RHScount++] = pivotRow;
                RHSarray[pivotRow] = pivotX;
                const int start = Ustart[iLogic];
                const int end = Uend[iLogic];
                if (iLogic >= numRow)
                    RHS_Tick += (end - start);
                for (int k = start; k < end; k++)
                    RHSarray[Uindex[k]] -= pivotX * Uvalue[k];
            } else
                RHSarray[pivotRow] = 0;
        }
        // Save the count
        rhs.count = RHScount;
        rhs.pseudoTick = RHS_Tick + (UpivotCount - numRow);
        rhs.fakeTick += (RHS_Tick - RHS_start) * 15
                + (UpivotCount - numRow) * 10;
    } else {
        solveHyper(numRow, &UpivotLookup[0], &UpivotIndex[0], &UpivotValue[0],
                &Ustart[0], &Ulastp[0], &Uindex[0], &Uvalue[0], &rhs);
    }
    if (updateMethod == UPDATE_METHOD_PF) {
        ftranPF(rhs);
        rhs.tight();
        rhs.pack();
    }
}
void HFactor::btranU(HVector& rhs, double hist_dsty) const {
    if (updateMethod == UPDATE_METHOD_PF) {
        btranPF(rhs);
    }
    // The regular part
    const double hyperBTRANU = 0.15;
    const double hyperCANCEL = 0.05;
    double curr_dsty = 1.0 * rhs.count / numRow;
    if (curr_dsty > hyperCANCEL || hist_dsty > hyperBTRANU) {
        // Alias to non constant
        int RHS_Tick = rhs.pseudoTick;
        int RHScount = 0;
        int *RHSindex = &rhs.index[0];
        double *RHSarray = &rhs.array[0];
        // Alias to the factor
        const int *URstart = &this->URstart[0];
        const int *URend = &this->URlastp[0];
        const int *URindex = &this->URindex[0];
        const double *URvalue = &this->URvalue[0];
        // Transform
        double RHS_start = RHS_Tick;
        int UpivotCount = UpivotIndex.size();
        for (int iLogic = 0; iLogic < UpivotCount; iLogic++) {
            // Skip void
            if (UpivotIndex[iLogic] == -1)
                continue;
            // Normal part
            const int pivotRow = UpivotIndex[iLogic];
            double pivotX = RHSarray[pivotRow];
            if (fabs(pivotX) > HSOL_CONST_TINY) {
                pivotX /= UpivotValue[iLogic];
                RHSindex[RHScount++] = pivotRow;
                RHSarray[pivotRow] = pivotX;
                const int start = URstart[iLogic];
                const int end = URend[iLogic];
                if (iLogic >= numRow)
                    RHS_Tick += end - start;
                for (int k = start; k < end; k++)
                    RHSarray[URindex[k]] -= pivotX * URvalue[k];
            } else
                RHSarray[pivotRow] = 0;
        }
        // Save the count
        rhs.count = RHScount;
        rhs.pseudoTick = RHS_Tick + (UpivotCount - numRow);
        rhs.fakeTick += (RHS_Tick - RHS_start) * 15
                + (UpivotCount - numRow) * 10;
    } else {
        solveHyper(numRow, &UpivotLookup[0], &UpivotIndex[0], &UpivotValue[0],
                &URstart[0], &URlastp[0], &URindex[0], &URvalue[0], &rhs);
    }
    // The update part
    if (updateMethod == UPDATE_METHOD_FT) {
        rhs.tight();
        rhs.pack();
        btranFT(rhs);
        rhs.tight();
    }
    if (updateMethod == UPDATE_METHOD_MPF) {
        rhs.tight();
        rhs.pack();
        btranMPF(rhs);
        rhs.tight();
    }
}
void HFactor::ftranFT(HVector& vector) const {
    // Alias to PF buffer
    const int PFpivotCount = PFpivotIndex.size();
    const int *PFpivotIndex = &this->PFpivotIndex[0];
    const int *PFstart = &this->PFstart[0];
    const int *PFindex = &this->PFindex[0];
    const double *PFvalue = &this->PFvalue[0];
    // Alias to non constant
    int RHS_tick = vector.pseudoTick;
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];
    // Forwardly apply row ETA
    for (int i = 0; i < PFpivotCount; i++) {
        int iRow = PFpivotIndex[i];
        double value0 = RHSarray[iRow];
        double value1 = value0;
        const int start = PFstart[i];
        const int end = PFstart[i + 1];
        RHS_tick += end - start;
        for (int k = start; k < end; k++)
            value1 -= RHSarray[PFindex[k]] * PFvalue[k];
        // This would skip the situation where they are both zeros
        if (value0 || value1) {
            if (value0 == 0)
                RHSindex[RHScount++] = iRow;
            RHSarray[iRow] =
                    (fabs(value1) < HSOL_CONST_TINY) ? HSOL_CONST_ZERO : value1;
        }
    }
    // Save count back
    vector.count = RHScount;
    vector.pseudoTick = RHS_tick + PFpivotCount * 0.5;
    vector.fakeTick += PFpivotCount * 20 + PFstart[PFpivotCount] * 5;
    if (PFstart[PFpivotCount] / (PFpivotCount + 1) < 5)
        vector.fakeTick += PFstart[PFpivotCount] * 5;
}
void HFactor::btranFT(HVector& vector) const {
    // Alias to PF buffer
    const int PFpivotCount = PFpivotIndex.size();
    const int *PFpivotIndex = &this->PFpivotIndex[0];
    const int *PFstart = &this->PFstart[0];
    const int *PFindex = &this->PFindex[0];
    const double *PFvalue = &this->PFvalue[0];
    // Alias to non constant
    int RHS_tick = vector.pseudoTick;
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];
    // Backwardly apply row ETA
    double RHS_start = RHS_tick;
    for (int i = PFpivotCount - 1; i >= 0; i--) {
        int pivotRow = PFpivotIndex[i];
        double pivotX = RHSarray[pivotRow];
        if (pivotX) {
            const int start = PFstart[i];
            const int end = PFstart[i + 1];
            RHS_tick += end - start;
            for (int k = start; k < end; k++) {
                int iRow = PFindex[k];
                double value0 = RHSarray[iRow];
                double value1 = value0 - pivotX * PFvalue[k];
                if (value0 == 0)
                    RHSindex[RHScount++] = iRow;
                RHSarray[iRow] =
                        (fabs(value1) < HSOL_CONST_TINY) ?
                                HSOL_CONST_ZERO : value1;
            }
        }
    }
    vector.fakeTick += (RHS_tick - RHS_start) * 15 + PFpivotCount * 10;
    // Save count back
    vector.pseudoTick = RHS_tick + PFpivotCount * 0.3;
    vector.count = RHScount;
}
void HFactor::ftranPF(HVector& vector) const {
    // Alias to PF buffer
    const int PFpivotCount = PFpivotIndex.size();
    const int *PFpivotIndex = &this->PFpivotIndex[0];
    const double *PFpivotValue = &this->PFpivotValue[0];
    const int *PFstart = &this->PFstart[0];
    const int *PFindex = &this->PFindex[0];
    const double *PFvalue = &this->PFvalue[0];
    // Alias to non constant
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];
    // Forwardly
    for (int i = 0; i < PFpivotCount; i++) {
        int pivotRow = PFpivotIndex[i];
        double pivotX = RHSarray[pivotRow];
        if (fabs(pivotX) > HSOL_CONST_TINY) {
            pivotX /= PFpivotValue[i];
            RHSarray[pivotRow] = pivotX;
            for (int k = PFstart[i]; k < PFstart[i + 1]; k++) {
                const int index = PFindex[k];
                const double value0 = RHSarray[index];
                const double value1 = value0 - pivotX * PFvalue[k];
                if (value0 == 0)
                    RHSindex[RHScount++] = index;
                RHSarray[index] =
                        (fabs(value1) < HSOL_CONST_TINY) ?
                                HSOL_CONST_ZERO : value1;
            }
        }
    }
    // Save count
    vector.count = RHScount;
}
void HFactor::btranPF(HVector& vector) const {
    // Alias to PF buffer
    const int PFpivotCount = PFpivotIndex.size();
    const int *PFpivotIndex = &this->PFpivotIndex[0];
    const double *PFpivotValue = &this->PFpivotValue[0];
    const int *PFstart = &this->PFstart[0];
    const int *PFindex = &this->PFindex[0];
    const double *PFvalue = &this->PFvalue[0];
    // Alias to non constant
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];
    // Backwardly
    for (int i = PFpivotCount - 1; i >= 0; i--) {
        int pivotRow = PFpivotIndex[i];
        double pivotX = RHSarray[pivotRow];
        for (int k = PFstart[i]; k < PFstart[i + 1]; k++)
            pivotX -= PFvalue[k] * RHSarray[PFindex[k]];
        pivotX /= PFpivotValue[i];
        if (RHSarray[pivotRow] == 0)
            RHSindex[RHScount++] = pivotRow;
        RHSarray[pivotRow] = (fabs(pivotX) < HSOL_CONST_TINY) ? 1e-100 : pivotX;
    }
    // Save count
    vector.count = RHScount;
}
void HFactor::ftranMPF(HVector& vector) const {
    // Alias to non constant
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];
    // Forwardly
    int PFpivotCount = PFpivotValue.size();
    for (int i = 0; i < PFpivotCount; i++) {
        solveMatrixT(PFstart[i * 2 + 1], PFstart[i * 2 + 2], PFstart[i * 2],
                PFstart[i * 2 + 1], &PFindex[0], &PFvalue[0], PFpivotValue[i],
                &RHScount, RHSindex, RHSarray);
    }
    // Remove cancellation
    vector.count = RHScount;
}
void HFactor::btranMPF(HVector& vector) const {
    // Alias to non constant
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];
    // Backwardly
    for (int i = PFpivotValue.size() - 1; i >= 0; i--) {
        solveMatrixT(PFstart[i * 2], PFstart[i * 2 + 1], PFstart[i * 2 + 1],
                PFstart[i * 2 + 2], &PFindex[0], &PFvalue[0], PFpivotValue[i],
                &RHScount, RHSindex, RHSarray);
    }
    // Remove cancellation
    vector.count = RHScount;
}
void HFactor::ftranAPF(HVector& vector) const {
    // Alias to non constant
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];
    // Backwardly
    int PFpivotCount = PFpivotValue.size();
    for (int i = PFpivotCount - 1; i >= 0; i--) {
        solveMatrixT(PFstart[i * 2 + 1], PFstart[i * 2 + 2], PFstart[i * 2],
                PFstart[i * 2 + 1], &PFindex[0], &PFvalue[0], PFpivotValue[i],
                &RHScount, RHSindex, RHSarray);
    }
    // Remove cancellation
    vector.count = RHScount;
}
void HFactor::btranAPF(HVector& vector) const {
    // Alias to non constant
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];
    // Forwardly
    int PFpivotCount = PFpivotValue.size();
    for (int i = 0; i < PFpivotCount; i++) {
        solveMatrixT(PFstart[i * 2], PFstart[i * 2 + 1], PFstart[i * 2 + 1],
                PFstart[i * 2 + 2], &PFindex[0], &PFvalue[0], PFpivotValue[i],
                &RHScount, RHSindex, RHSarray);
    }
    vector.count = RHScount;
}
void HFactor::updateCFT(HVector *aq, HVector *ep, int *iRow, int *) {
    /*
     * In the major update loop, the prefix
     *
     * c(p) = current working pivot
     * p(p) = previous pivot  (0 =< pp < cp)
     */
    int numUpdate = 0;
    for (HVector *vec = aq; vec != 0; vec = vec->next)
        numUpdate++;
    HVector* aqWork[numUpdate];
    HVector* epWork[numUpdate];
    for (int i = 0; i < numUpdate; i++) {
        aqWork[i] = aq;
        epWork[i] = ep;
        aq = aq->next;
        ep = ep->next;
    }
    // Pivot related buffers
    int PFnp0 = PFpivotIndex.size();
    int pLogic[numUpdate];
    double pValue[numUpdate];
    double pAlpha[numUpdate];
    for (int cp = 0; cp < numUpdate; cp++) {
        int cRow = iRow[cp];
        int iLogic = UpivotLookup[cRow];
        pLogic[cp] = iLogic;
        pValue[cp] = UpivotValue[iLogic];
        pAlpha[cp] = aqWork[cp]->array[cRow];
    }
    // Temporary U pointers
    int Tstart[numUpdate + 1];
    double Tpivot[numUpdate];
    Tstart[0] = Uindex.size();
    // Logically sorted previous row_ep
    vector<pair<int, int> > sorted_pp;
    // Major update loop
    for (int cp = 0; cp < numUpdate; cp++) {
        // 1. Expand partial FTRAN result to buffer
        iwork.clear();
        for (int i = 0; i < aqWork[cp]->packCount; i++) {
            int index = aqWork[cp]->packIndex[i];
            double value = aqWork[cp]->packValue[i];
            iwork.push_back(index);
            dwork[index] = value;
        }
        // 2. Update partial FTRAN result by recent FT matrix
        for (int pp = 0; pp < cp; pp++) {
            int pRow = iRow[pp];
            double value = dwork[pRow];
            int PFpp = pp + PFnp0;
            for (int i = PFstart[PFpp]; i < PFstart[PFpp + 1]; i++)
                value -= dwork[PFindex[i]] * PFvalue[i];
            iwork.push_back(pRow); // OK to duplicate
            dwork[pRow] = value;
        }
        // 3. Store the partial FTRAN result to matirx U
        double ppaq = dwork[iRow[cp]]; // pivot of the partial aq
        dwork[iRow[cp]] = 0;
        int UcountX = Tstart[cp];
        int UstartX = UcountX;
        for (unsigned i = 0; i < iwork.size(); i++) {
            int index = iwork[i];
            double value = dwork[index];
            dwork[index] = 0; // This effectively removes all duplication
            if (fabs(value) > HSOL_CONST_TINY) {
                Uindex.push_back(index);
                Uvalue.push_back(value);
            }
        }
        UcountX = Uindex.size();
        Tstart[cp + 1] = UcountX;
        Tpivot[cp] = pValue[cp] * pAlpha[cp];
        // 4. Expand partial BTRAN result to buffer
        iwork.clear();
        for (int i = 0; i < epWork[cp]->packCount; i++) {
            int index = epWork[cp]->packIndex[i];
            double value = epWork[cp]->packValue[i];
            iwork.push_back(index);
            dwork[index] = value;
        }
        // 5. Delete logical later rows (in logical order)
        for (int isort = 0; isort < cp; isort++) {
            int pp = sorted_pp[isort].second;
            int pRow = iRow[pp];
            double multiplier = -pValue[pp] * dwork[pRow];
            if (fabs(dwork[pRow]) > HSOL_CONST_TINY) {
                for (int i = 0; i < epWork[pp]->packCount; i++) {
                    int index = epWork[pp]->packIndex[i];
                    double value = epWork[pp]->packValue[i];
                    iwork.push_back(index);
                    dwork[index] += value * multiplier;
                }
            }
            dwork[pRow] = 0; // Force to be 0
        }
        // 6. Update partial BTRAN result by recent U columns
        for (int pp = 0; pp < cp; pp++) {
            int kpivot = iRow[pp];
            double value = dwork[kpivot];
            for (int k = Tstart[pp]; k < Tstart[pp + 1]; k++)
                value -= dwork[Uindex[k]] * Uvalue[k];
            value /= Tpivot[pp];
            iwork.push_back(kpivot);
            dwork[kpivot] = value; // Again OK to duplicate
        }
        // 6.x compute current alpha
        double thex = 0;
        for (int k = UstartX; k < UcountX; k++) {
            int index = Uindex[k];
            double value = Uvalue[k];
            thex += dwork[index] * value;
        }
        Tpivot[cp] = ppaq + thex * pValue[cp];
        // 7. Store BTRAN result to FT elimination, update logic helper
        dwork[iRow[cp]] = 0;
        double pivotX = -pValue[cp];
        for (unsigned i = 0; i < iwork.size(); i++) {
            int index = iwork[i];
            double value = dwork[index];
            dwork[index] = 0;
            if (fabs(value) > HSOL_CONST_TINY) {
                PFindex.push_back(index);
                PFvalue.push_back(value * pivotX);
            }
        }
        PFpivotIndex.push_back(iRow[cp]);
        FtotalX += PFindex.size() - PFstart.back() + 1;
        UtotalX += PFindex.size() - PFstart.back();
        PFstart.push_back(PFindex.size());
        // 8. Update the sorted ep
        sorted_pp.push_back(make_pair(pLogic[cp], cp));
        sort(sorted_pp.begin(), sorted_pp.end());
    }
    // Now modify the U matrix
    for (int cp = 0; cp < numUpdate; cp++) {
        // 1. Delete pivotal row from U
        int cIndex = iRow[cp];
        int cLogic = pLogic[cp];
        FtotalX -= URlastp[cLogic] - URstart[cLogic];
        UtotalX -= URlastp[cLogic] - URstart[cLogic];
        for (int k = URstart[cLogic]; k < URlastp[cLogic]; k++) {
            // Find the pivotal position
            int iLogic = UpivotLookup[URindex[k]];
            int iFind = Ustart[iLogic];
            int iLast = --Ulastp[iLogic];
            for (; iFind <= iLast; iFind++)
                if (Uindex[iFind] == cIndex)
                    break;
            // Put last to find, and delete last
            Uindex[iFind] = Uindex[iLast];
            Uvalue[iFind] = Uvalue[iLast];
        }
        // 2. Delete pivotal column from UR
        FtotalX -= Ulastp[cLogic] - Ustart[cLogic];
        UtotalX -= Ulastp[cLogic] - Ustart[cLogic];
        for (int k = Ustart[cLogic]; k < Ulastp[cLogic]; k++) {
            // Find the pivotal position
            int iLogic = UpivotLookup[Uindex[k]];
            int iFind = URstart[iLogic];
            int iLast = --URlastp[iLogic];
            for (; iFind <= iLast; iFind++)
                if (URindex[iFind] == cIndex)
                    break;
            // Put last to find, and delete last
            URspace[iLogic]++;
            URindex[iFind] = URindex[iLast];
            URvalue[iFind] = URvalue[iLast];
        }
        // 3. Insert the (stored) partial FTRAN to the row matrix
        int UstartX = Tstart[cp];
        int UendX = Tstart[cp + 1];
        FtotalX += UendX - UstartX + 1;
        UtotalX += UendX - UstartX;
        // Store column as UR elements
        for (int k = UstartX; k < UendX; k++) {
            // Which ETA file
            int iLogic = UpivotLookup[Uindex[k]];
            // Move row to the end if necessary
            if (URspace[iLogic] == 0) {
                // Make pointers
                int row_start = URstart[iLogic];
                int row_count = URlastp[iLogic] - row_start;
                int new_start = URindex.size();
                int new_space = row_count * 1.1 + 5;
                // Check matrix UR
                URindex.resize(new_start + new_space);
                URvalue.resize(new_start + new_space);
                // Move elements
                int iFrom = row_start;
                int iEnd = row_start + row_count;
                int iTo = new_start;
                copy(&URindex[iFrom], &URindex[iEnd], &URindex[iTo]);
                copy(&URvalue[iFrom], &URvalue[iEnd], &URvalue[iTo]);
                // Save new pointers
                URstart[iLogic] = new_start;
                URlastp[iLogic] = new_start + row_count;
                URspace[iLogic] = new_space - row_count;
            }
            // Put into the next available space
            URspace[iLogic]--;
            int iPut = URlastp[iLogic]++;
            URindex[iPut] = cIndex;
            URvalue[iPut] = Uvalue[k];
        }
        // 4. Save pointers
        Ustart.push_back(UstartX);
        Ulastp.push_back(UendX);
        URstart.push_back(URstart[cLogic]);
        URlastp.push_back(URstart[cLogic]);
        URspace.push_back(URspace[cLogic] + URlastp[cLogic] - URstart[cLogic]);
        UpivotLookup[cIndex] = UpivotIndex.size();
        UpivotIndex[cLogic] = -1;
        UpivotIndex.push_back(cIndex);
        UpivotValue.push_back(Tpivot[cp]);
    }
//    // See if we want refactor
//    if (UtotalX > UmeritX && PFpivotIndex.size() > 100)
//        *hint = 1;
}
void HFactor::updateFT(HVector *aq, HVector *ep, int iRow, int *) {
    // Store pivot
    int pLogic = UpivotLookup[iRow];
    double pivot = UpivotValue[pLogic];
    double alpha = aq->array[iRow];
    UpivotIndex[pLogic] = -1;
    // Delete pivotal row from U
    FtotalX -= URlastp[pLogic] - URstart[pLogic];
    for (int k = URstart[pLogic]; k < URlastp[pLogic]; k++) {
        // Find the pivotal position
        int iLogic = UpivotLookup[URindex[k]];
        int iFind = Ustart[iLogic];
        int iLast = --Ulastp[iLogic];
        for (; iFind <= iLast; iFind++)
            if (Uindex[iFind] == iRow)
                break;
        // Put last to find, and delete last
        Uindex[iFind] = Uindex[iLast];
        Uvalue[iFind] = Uvalue[iLast];
    }
    // Delete pivotal column from UR
    FtotalX -= Ulastp[pLogic] - Ustart[pLogic];
    for (int k = Ustart[pLogic]; k < Ulastp[pLogic]; k++) {
        // Find the pivotal position
        int iLogic = UpivotLookup[Uindex[k]];
        int iFind = URstart[iLogic];
        int iLast = --URlastp[iLogic];
        for (; iFind <= iLast; iFind++)
            if (URindex[iFind] == iRow)
                break;
        // Put last to find, and delete last
        URspace[iLogic]++;
        URindex[iFind] = URindex[iLast];
        URvalue[iFind] = URvalue[iLast];
    }
    // Store column to U
    Ustart.push_back(Uindex.size());
    for (int i = 0; i < aq->packCount; i++)
        if (aq->packIndex[i] != iRow) {
            Uindex.push_back(aq->packIndex[i]);
            Uvalue.push_back(aq->packValue[i]);
        }
    Ulastp.push_back(Uindex.size());
    int UstartX = Ustart.back();
    int UendX = Ulastp.back();
    UtotalX += UendX - UstartX + 1;
    FtotalX += UendX - UstartX + 1;
    // Store column as UR elements
    for (int k = UstartX; k < UendX; k++) {
        // Which ETA file
        int iLogic = UpivotLookup[Uindex[k]];
        // Move row to the end if necessary
        if (URspace[iLogic] == 0) {
            // Make pointers
            int row_start = URstart[iLogic];
            int row_count = URlastp[iLogic] - row_start;
            int new_start = URindex.size();
            int new_space = row_count * 1.1 + 5;
            // Check matrix UR
            URindex.resize(new_start + new_space);
            URvalue.resize(new_start + new_space);
            // Move elements
            int iFrom = row_start;
            int iEnd = row_start + row_count;
            int iTo = new_start;
            copy(&URindex[iFrom], &URindex[iEnd], &URindex[iTo]);
            copy(&URvalue[iFrom], &URvalue[iEnd], &URvalue[iTo]);
            // Save new pointers
            URstart[iLogic] = new_start;
            URlastp[iLogic] = new_start + row_count;
            URspace[iLogic] = new_space - row_count;
        }
        // Put into the next available space
        URspace[iLogic]--;
        int iPut = URlastp[iLogic]++;
        URindex[iPut] = iRow;
        URvalue[iPut] = Uvalue[k];
    }
    // Store UR pointers
    URstart.push_back(URstart[pLogic]);
    URlastp.push_back(URstart[pLogic]);
    URspace.push_back(URspace[pLogic] + URlastp[pLogic] - URstart[pLogic]);
    // Update pivot count
    UpivotLookup[iRow] = UpivotIndex.size();
    UpivotIndex.push_back(iRow);
    UpivotValue.push_back(pivot * alpha);
    // Store row_ep as R matrix
    for (int i = 0; i < ep->packCount; i++) {
        if (ep->packIndex[i] != iRow) {
            PFindex.push_back(ep->packIndex[i]);
            PFvalue.push_back(-ep->packValue[i] * pivot);
        }
    }
    UtotalX += PFindex.size() - PFstart.back();
    FtotalX += PFindex.size() - PFstart.back() + 1;
    // Store R matrix pivot
    PFpivotIndex.push_back(iRow);
    PFstart.push_back(PFindex.size());
    // Update total countX
    UtotalX -= Ulastp[pLogic] - Ustart[pLogic];
    UtotalX -= URlastp[pLogic] - URstart[pLogic];
//    // See if we want refactor
//    if (UtotalX > UmeritX && PFpivotIndex.size() > 100)
//        *hint = 1;
}
void HFactor::updatePF(HVector *aq, HVector *, int iRow, int *hint) {
    // Check space
    const int columnCount = aq->packCount;
    const int *columnIndex = &aq->packIndex[0];
    const double *columnArray = &aq->packValue[0];
    // Copy the pivotal column
    for (int i = 0; i < columnCount; i++) {
        int index = columnIndex[i];
        double value = columnArray[i];
        if (index != iRow) {
            PFindex.push_back(index);
            PFvalue.push_back(value);
        }
    }
    // Save pivot
    PFpivotIndex.push_back(iRow);
    PFpivotValue.push_back(aq->array[iRow]);
    PFstart.push_back(PFindex.size());
    // Check refactor
    UtotalX += aq->packCount;
    if (UtotalX > UmeritX)
        *hint = 1;
    FtotalX += aq->packCount;
}
void HFactor::updateMPF(HVector *aq, HVector *ep, int iRow, int *hint) {
    int PFcountX0 = PFindex.size();
    // Store elements
    for (int i = 0; i < aq->packCount; i++) {
        PFindex.push_back(aq->packIndex[i]);
        PFvalue.push_back(aq->packValue[i]);
    }
    int pLogic = UpivotLookup[iRow];
    int UstartX = Ustart[pLogic];
    int UendX = Ustart[pLogic + 1];
    for (int k = UstartX; k < UendX; k++) {
        PFindex.push_back(Uindex[k]);
        PFvalue.push_back(-Uvalue[k]);
    }
    PFindex.push_back(iRow);
    PFvalue.push_back(-UpivotValue[pLogic]);
    PFstart.push_back(PFindex.size());
    for (int i = 0; i < ep->packCount; i++) {
        PFindex.push_back(ep->packIndex[i]);
        PFvalue.push_back(ep->packValue[i]);
    }
    PFstart.push_back(PFindex.size());
    // Store pivot
    PFpivotValue.push_back(aq->array[iRow]);
    // Refactor or not
    UtotalX += aq->packCount + ep->packCount;
    FtotalX += PFindex.size() - PFcountX0;
    if (UtotalX > UmeritX)
        *hint = 1;
}
void HFactor::updateAPF(HVector *aq, HVector *ep, int iRow, int *) {
    int PFcountX0 = PFindex.size();
    // Store elements
    for (int i = 0; i < aq->packCount; i++) {
        PFindex.push_back(aq->packIndex[i]);
        PFvalue.push_back(aq->packValue[i]);
    }
    int columnOut = baseIndex[iRow];
    if (columnOut >= numCol) {
        PFindex.push_back(columnOut - numCol);
        PFvalue.push_back(-1);
    } else {
        for (int k = Astart[columnOut]; k < Astart[columnOut + 1]; k++) {
            PFindex.push_back(Aindex[k]);
            PFvalue.push_back(-Avalue[k]);
        }
    }
    PFstart.push_back(PFindex.size());
    for (int i = 0; i < ep->packCount; i++) {
        PFindex.push_back(ep->packIndex[i]);
        PFvalue.push_back(ep->packValue[i]);
    }
    PFstart.push_back(PFindex.size());
    FtotalX += PFindex.size() - PFcountX0;
    // Store pivot
    PFpivotValue.push_back(aq->array[iRow]);
}
// #include <cmath>
void HMatrix::setup(int numCol_, int numRow_, const int *Astart_,
        const int *Aindex_, const double *Avalue_) {
    // Copy A
    numCol = numCol_;
    numRow = numRow_;
    Astart.assign(Astart_, Astart_ + numCol_ + 1);
    int AcountX = Astart_[numCol_];
    Aindex.assign(Aindex_, Aindex_ + AcountX);
    Avalue.assign(Avalue_, Avalue_ + AcountX);
    // Build row copy - pointers
    ARstart.resize(numRow + 1);
    AR_Nend.assign(numRow, 0);
    for (int k = 0; k < AcountX; k++)
        AR_Nend[Aindex[k]]++;
    ARstart[0] = 0;
    for (int i = 1; i <= numRow; i++)
        ARstart[i] = ARstart[i - 1] + AR_Nend[i - 1];
    for (int i = 0; i < numRow; i++)
        AR_Nend[i] = ARstart[i];
    // Build row copy - elements
    ARindex.resize(AcountX);
    ARvalue.resize(AcountX);
    for (int iCol = 0; iCol < numCol; iCol++) {
        for (int k = Astart[iCol]; k < Astart[iCol + 1]; k++) {
            int iRow = Aindex[k];
            int iPut = AR_Nend[iRow]++;
            ARindex[iPut] = iCol;
            ARvalue[iPut] = Avalue[k];
        }
    }
}
void HMatrix::update(int columnIn, int columnOut) {
    if (columnIn < numCol) {
        for (int k = Astart[columnIn]; k < Astart[columnIn + 1]; k++) {
            int iRow = Aindex[k];
            int iFind = ARstart[iRow];
            int iSwap = --AR_Nend[iRow];
            while (ARindex[iFind] != columnIn)
                iFind++;
            swap(ARindex[iFind], ARindex[iSwap]);
            swap(ARvalue[iFind], ARvalue[iSwap]);
        }
    }
    if (columnOut < numCol) {
        for (int k = Astart[columnOut]; k < Astart[columnOut + 1]; k++) {
            int iRow = Aindex[k];
            int iFind = AR_Nend[iRow];
            int iSwap = AR_Nend[iRow]++;
            while (ARindex[iFind] != columnOut)
                iFind++;
            swap(ARindex[iFind], ARindex[iSwap]);
            swap(ARvalue[iFind], ARvalue[iSwap]);
        }
    }
}
double HMatrix::compute_dot(HVector& vector, int iCol) const {
    double result = 0;
    if (iCol < numCol) {
        for (int k = Astart[iCol]; k < Astart[iCol + 1]; k++)
            result += vector.array[Aindex[k]] * Avalue[k];
    } else {
        result = vector.array[iCol - numCol];
    }
    return result;
}
void HMatrix::collect_aj(HVector& vector, int iCol, double multi) const {
    if (iCol < numCol) {
        for (int k = Astart[iCol]; k < Astart[iCol + 1]; k++) {
            int index = Aindex[k];
            double value0 = vector.array[index];
            double value1 = value0 + multi * Avalue[k];
            if (value0 == 0)
                vector.index[vector.count++] = index;
            vector.array[index] =
                    (fabs(value1) < HSOL_CONST_TINY) ? HSOL_CONST_ZERO : value1;
        }
    } else {
        int index = iCol - numCol;
        double value0 = vector.array[index];
        double value1 = value0 + multi;
        if (value0 == 0)
            vector.index[vector.count++] = index;
        vector.array[index] =
                (fabs(value1) < HSOL_CONST_TINY) ? HSOL_CONST_ZERO : value1;
    }
}
void HMatrix::price_by_col(HVector& row_ap, HVector& row_ep) const {
    // Alias
    int ap_count = 0;
    int *ap_index = &row_ap.index[0];
    double *ap_array = &row_ap.array[0];
    const double *ep_array = &row_ep.array[0];
    // Computation
    for (int iCol = 0; iCol < numCol; iCol++) {
        double value = 0;
        for (int k = Astart[iCol]; k < Astart[iCol + 1]; k++) {
            value += ep_array[Aindex[k]] * Avalue[k];
        }
        if (fabs(value) > HSOL_CONST_TINY) {
            ap_array[iCol] = value;
            ap_index[ap_count++] = iCol;
        }
    }
    row_ap.count = ap_count;
}
void HMatrix::price_by_row(HVector& row_ap, HVector& row_ep) const {
    // Alias
    int ap_count = 0;
    int *ap_index = &row_ap.index[0];
    double *ap_array = &row_ap.array[0];
    const int ep_count = row_ep.count;
    const int *ep_index = &row_ep.index[0];
    const double *ep_array = &row_ep.array[0];
    // Computation
    for (int i = 0; i < ep_count; i++) {
        int iRow = ep_index[i];
        double multi = ep_array[iRow];
        for (int k = ARstart[iRow]; k < AR_Nend[iRow]; k++) {
            int index = ARindex[k];
            double value0 = ap_array[index];
            double value1 = value0 + multi * ARvalue[k];
            if (value0 == 0)
                ap_index[ap_count++] = index;
            ap_array[index] =
                    (fabs(value1) < HSOL_CONST_TINY) ? HSOL_CONST_ZERO : value1;
        }
    }
    // Try to remove cancellation
    const int apcount1 = ap_count;
    ap_count = 0;
    for (int i = 0; i < apcount1; i++) {
        const int index = ap_index[i];
        const double value = ap_array[index];
        if (fabs(value) > HSOL_CONST_TINY) {
            ap_index[ap_count++] = index;
        } else {
            ap_array[index] = 0;
        }
    }
    row_ap.count = ap_count;
}
void HMatrix::compute_vecT_matB(const double *vec, const int *base,
        HVector *result) {
    result->clear();
    int resultCount = 0;
    int *resultIndex = &result->index[0];
    double *resultArray = &result->array[0];
    for (int i = 0; i < numRow; i++) {
        int iCol = base[i];
        double value = 0;
        if (iCol < numCol) {
            for (int k = Astart[iCol]; k < Astart[iCol + 1]; k++)
                value += vec[Aindex[k]] * Avalue[k];
        } else {
            value = vec[iCol - numCol];
        }
        if (fabs(value) > HSOL_CONST_TINY) {
            resultArray[i] = value;
            resultIndex[resultCount++] = i;
        }
    }
    result->count = resultCount;
}
void HMatrix::compute_matB_vec(const double *vec, const int *base,
        HVector *result) {
    result->clear();
    int resultCount = 0;
    int *resultIndex = &result->index[0];
    double *resultArray = &result->array[0];
    for (int i = 0; i < numRow; i++) {
        int iCol = base[i];
        double value = vec[i];
        if (fabs(value) > HSOL_CONST_TINY) {
            if (iCol < numCol) {
                for (int k = Astart[iCol]; k < Astart[iCol + 1]; k++)
                    resultArray[Aindex[k]] += value * Avalue[k];
            } else {
                resultArray[iCol - numCol] += value;
            }
        }
    }
    for (int i = 0; i < numRow; i++) {
        if (fabs(resultArray[i]) > HSOL_CONST_TINY) {
            resultIndex[resultCount++] = i;
        } else {
            resultArray[i] = 0;
        }
    }
    result->count = resultCount;
}
// #include <cctype>
// #include <cmath>
// #include <cassert>
// #include <cstdio>
// #include <cstdlib>
// #include <cstring>
// #include <algorithm>
// #include <map>
// #include <set>
// #include <fstream>
// #include <iomanip>
// #include <iostream>
// #include <stdexcept>
using namespace std;
HModel::HModel() {
    intOption[INTOPT_PRINT_FLAG] = 0; // no print
    intOption[INTOPT_TRANSPOSE_FLAG] = 0; // no transpose
    intOption[INTOPT_SCALE_FLAG] = 1; // do scale
    intOption[INTOPT_TIGHT_FLAG] = 1; // do tight
    intOption[INTOPT_PERMUTE_FLAG] = 0; // no permute
    intOption[INTOPT_PERTURB_FLAG] = 0; // do perturb
    dblOption[DBLOPT_PRIMAL_TOL] = 1e-7;
    dblOption[DBLOPT_DUAL_TOL] = 1e-7;
    dblOption[DBLOPT_PAMI_CUTOFF] = 0.95;
    strOption[STROPT_PARTITION_FILE] = "";
}
void HModel::setupstart() {
    problemStatus = -1;
    numberIteration = 0;
}
void HModel::setupend() {
  // Check size
  numTot = numCol + numRow;
  if (numRow == 0)
    return;
  // Setup random buffers: shuffle the break
  intBreak.resize(numTot);
  for (int i = 0; i < numTot; i++)
    intBreak[i] = i;
  for (int i = numTot - 1; i >= 1; i--) {
    int j = random.intRandom() % (i + 1);
    swap(intBreak[i], intBreak[j]);
  }
  dblXpert.resize(numTot);
  for (int i = 0; i < numTot; i++)
    dblXpert[i] = random.dblRandom();
  // Setup other part
  setup_transposeLP();
  setup_scaleMatrix();
  //    setup_tightenBound();
  //    setup_shuffleColumn();
  setup_allocWorking();
  // Initialize the values
  initCost();
  initBound();
  initValue();
}
void HModel::setup_transposeLP() {
    if (intOption[INTOPT_TRANSPOSE_FLAG] == 0)
        return;
    int transposeCancelled = 0;
    if (1.0 * numCol / numRow > 0.2) {
//        cout << "transpose-cancelled-by-ratio" << endl;
        transposeCancelled = 1;
        return;
    }
    // Convert primal cost to dual bound
    const double inf = HSOL_CONST_INF;
    vector<double> dualRowLower(numCol);
    vector<double> dualRowUpper(numCol);
    for (int j = 0; j < numCol; j++) {
        double lower = colLower[j];
        double upper = colUpper[j];
        /*
         * Primal      Dual
         * Free        row = c
         * x > 0       row < c
         * x < 0       row > c
         * x = 0       row free
         * other       cancel
         */
        if (lower == -inf && upper == inf) {
            dualRowLower[j] = colCost[j];
            dualRowUpper[j] = colCost[j];
        } else if (lower == 0 && upper == inf) {
            dualRowLower[j] = -inf;
            dualRowUpper[j] = colCost[j];
        } else if (lower == -inf && upper == 0) {
            dualRowLower[j] = colCost[j];
            dualRowUpper[j] = +inf;
        } else if (lower == 0 && upper == 0) {
            dualRowLower[j] = -inf;
            dualRowUpper[j] = +inf;
        } else {
            transposeCancelled = 1;
            break;
        }
    }
    // Check flag
    if (transposeCancelled == 1) {
//        cout << "transpose-cancelled-by-column" << endl;
        return;
    }
    // Convert primal row bound to dual variable cost
    vector<double> dualColLower(numRow);
    vector<double> dualColUpper(numRow);
    vector<double> dualCost(numRow);
    for (int i = 0; i < numRow; i++) {
        double lower = rowLower[i];
        double upper = rowUpper[i];
        /*
         * Primal      Dual
         * row = b     Free
         * row < b     y < 0
         * row > b     y > 0
         * row free    y = 0
         * other       cancel
         */
        if (lower == upper) {
            dualColLower[i] = -inf;
            dualColUpper[i] = +inf;
            dualCost[i] = -lower;
        } else if (lower == -inf && upper != inf) {
            dualColLower[i] = -inf;
            dualColUpper[i] = 0;
            dualCost[i] = -upper;
        } else if (lower != -inf && upper == inf) {
            dualColLower[i] = 0;
            dualColUpper[i] = +inf;
            dualCost[i] = -lower;
        } else if (lower == -inf && upper == inf) {
            dualColLower[i] = 0;
            dualColUpper[i] = 0;
            dualCost[i] = 0;
        } else {
            transposeCancelled = 1;
            break;
        }
    }
    // Check flag
    if (transposeCancelled == 1) {
//        cout << "transpose-cancelled-by-row" << endl;
        return;
    }
    // We can now really transpose things
    vector<int> iwork(numRow, 0);
    vector<int> ARstart(numRow + 1, 0);
    int AcountX = Aindex.size();
    vector<int> ARindex(AcountX);
    vector<double> ARvalue(AcountX);
    for (int k = 0; k < AcountX; k++)
        iwork[Aindex[k]]++;
    for (int i = 1; i <= numRow; i++)
        ARstart[i] = ARstart[i - 1] + iwork[i - 1];
    for (int i = 0; i < numRow; i++)
        iwork[i] = ARstart[i];
    for (int iCol = 0; iCol < numCol; iCol++) {
        for (int k = Astart[iCol]; k < Astart[iCol + 1]; k++) {
            int iRow = Aindex[k];
            int iPut = iwork[iRow]++;
            ARindex[iPut] = iCol;
            ARvalue[iPut] = Avalue[k];
        }
    }
    // Transpose the problem!
    swap(numRow, numCol);
    Astart.swap(ARstart);
    Aindex.swap(ARindex);
    Avalue.swap(ARvalue);
    colLower.swap(dualColLower);
    colUpper.swap(dualColUpper);
    rowLower.swap(dualRowLower);
    rowUpper.swap(dualRowUpper);
    colCost.swap(dualCost);
//    cout << "problem-transposed" << endl;
}
void HModel::setup_scaleMatrix() {
    if (intOption[INTOPT_SCALE_FLAG] == 0)
        return;
    // Reset all scaling to 1
    vector<double> colScale(numCol, 1);
    vector<double> rowScale(numRow, 1);
    // Find out min0 / max0, skip on if in [0.2, 5]
    const double inf = HSOL_CONST_INF;
    double min0 = inf, max0 = 0;
    for (int k = 0, AnX = Astart[numCol]; k < AnX; k++) {
        double value = fabs(Avalue[k]);
        min0 = min(min0, value);
        max0 = max(max0, value);
    }
    if (min0 >= 0.2 && max0 <= 5)
        return;
    // See if we want to include cost include if min-cost < 0.1
    double minc = inf;
    for (int i = 0; i < numCol; i++)
        if (colCost[i])
            minc = min(minc, fabs(colCost[i]));
    bool doCost = minc < 0.1;
    // Search up to 6 times
    vector<double> rowMin(numRow, inf);
    vector<double> rowMax(numRow, 1 / inf);
    for (int search_count = 0; search_count < 6; search_count++) {
        // Find column scale, prepare row data
        for (int iCol = 0; iCol < numCol; iCol++) {
            // For column scale (find)
            double colMin = inf;
            double colMax = 1 / inf;
            double myCost = fabs(colCost[iCol]);
            if (doCost && myCost != 0)
                colMin = min(colMin, myCost), colMax = max(colMax, myCost);
            for (int k = Astart[iCol]; k < Astart[iCol + 1]; k++) {
                double value = fabs(Avalue[k]) * rowScale[Aindex[k]];
                colMin = min(colMin, value), colMax = max(colMax, value);
            }
            colScale[iCol] = 1 / sqrt(colMin * colMax);
            // For row scale (only collect)
            for (int k = Astart[iCol]; k < Astart[iCol + 1]; k++) {
                int iRow = Aindex[k];
                double value = fabs(Avalue[k]) * colScale[iCol];
                rowMin[iRow] = min(rowMin[iRow], value);
                rowMax[iRow] = max(rowMax[iRow], value);
            }
        }
        // For row scale (find)
        for (int iRow = 0; iRow < numRow; iRow++)
            rowScale[iRow] = 1 / sqrt(rowMin[iRow] * rowMax[iRow]);
        rowMin.assign(numRow, inf);
        rowMax.assign(numRow, 1 / inf);
    }
    // Make it numerical better
    const double ln2 = log(2.0);
    for (int iCol = 0; iCol < numCol; iCol++)
        colScale[iCol] = pow(2.0, floor(log(colScale[iCol]) / ln2 + 0.5));
    for (int iRow = 0; iRow < numRow; iRow++)
        rowScale[iRow] = pow(2.0, floor(log(rowScale[iRow]) / ln2 + 0.5));
    // Apply scaling to matrix and bounds
    for (int iCol = 0; iCol < numCol; iCol++)
        for (int k = Astart[iCol]; k < Astart[iCol + 1]; k++)
            Avalue[k] *= (colScale[iCol] * rowScale[Aindex[k]]);
    for (int iCol = 0; iCol < numCol; iCol++) {
        colLower[iCol] /= colLower[iCol] == -inf ? 1 : colScale[iCol];
        colUpper[iCol] /= colUpper[iCol] == +inf ? 1 : colScale[iCol];
        colCost[iCol] *= colScale[iCol];
    }
    for (int iRow = 0; iRow < numRow; iRow++) {
        rowLower[iRow] *= rowLower[iRow] == -inf ? 1 : rowScale[iRow];
        rowUpper[iRow] *= rowUpper[iRow] == +inf ? 1 : rowScale[iRow];
    }
}
void HModel::setup_tightenBound() {
    if (intOption[INTOPT_TIGHT_FLAG] == 0)
        return;
    // Make a AR copy
    vector<int> iwork(numRow, 0);
    vector<int> ARstart(numRow + 1, 0);
    int AcountX = Aindex.size();
    vector<int> ARindex(AcountX);
    vector<double> ARvalue(AcountX);
    for (int k = 0; k < AcountX; k++)
        iwork[Aindex[k]]++;
    for (int i = 1; i <= numRow; i++)
        ARstart[i] = ARstart[i - 1] + iwork[i - 1];
    for (int i = 0; i < numRow; i++)
        iwork[i] = ARstart[i];
    for (int iCol = 0; iCol < numCol; iCol++) {
        for (int k = Astart[iCol]; k < Astart[iCol + 1]; k++) {
            int iRow = Aindex[k];
            int iPut = iwork[iRow]++;
            ARindex[iPut] = iCol;
            ARvalue[iPut] = Avalue[k];
        }
    }
    // Save column bounds
    vector<double> colLower0 = colLower;
    vector<double> colUpper0 = colUpper;
    double big_B = 1e10;
    int iPass = 0;
    for (;;) {
        int numberChanged = 0;
        for (int iRow = 0; iRow < numRow; iRow++) {
            // SKIP free rows
            if (rowLower[iRow] < -big_B && rowUpper[iRow] > big_B)
                continue;
            // possible row
            int ninfU = 0;
            int ninfL = 0;
            double xmaxU = 0.0;
            double xminL = 0.0;
            int myStart = ARstart[iRow];
            int myEnd = ARstart[iRow + 1];
            // Compute possible lower and upper ranges
            for (int k = myStart; k < myEnd; ++k) {
                int iCol = ARindex[k];
                double value = ARvalue[k];
                double upper = value > 0 ? colUpper[iCol] : -colLower[iCol];
                double lower = value > 0 ? colLower[iCol] : -colUpper[iCol];
                value = fabs(value);
                if (upper < big_B)
                    xmaxU += upper * value;
                else
                    ++ninfU;
                if (lower > -big_B)
                    xminL += lower * value;
                else
                    ++ninfL;
            }
            // Build in a margin of error
            xmaxU += 1.0e-8 * fabs(xmaxU);
            xminL -= 1.0e-8 * fabs(xminL);
            double xminLmargin =
                    (fabs(xminL) > 1.0e8) ? 1e-12 * fabs(xminL) : 0;
            double xmaxUmargin =
                    (fabs(xmaxU) > 1.0e8) ? 1e-12 * fabs(xmaxU) : 0;
            // Skip redundant row : also need to consider U < L  case
            double comp_U = xmaxU + ninfU * 1.0e31;
            double comp_L = xminL - ninfL * 1.0e31;
            if (comp_U <= rowUpper[iRow] + 1e-7
                    && comp_L >= rowLower[iRow] - 1e-7)
                continue;
            double row_L = rowLower[iRow];
            double row_U = rowUpper[iRow];
            // Now see if we can tighten column bounds
            for (int k = myStart; k < myEnd; ++k) {
                double value = ARvalue[k];
                int iCol = ARindex[k];
                double col_L = colLower[iCol];
                double col_U = colUpper[iCol];
                double new_L = -HSOL_CONST_INF;
                double new_U = +HSOL_CONST_INF;
                if (value > 0.0) {
                    if (row_L > -big_B && ninfU <= 1
                            && (ninfU == 0 || col_U > +big_B))
                        new_L = (row_L - xmaxU) / value + (1 - ninfU) * col_U
                                - xmaxUmargin;
                    if (row_U < +big_B && ninfL <= 1
                            && (ninfL == 0 || col_L < -big_B))
                        new_U = (row_U - xminL) / value + (1 - ninfL) * col_L
                                + xminLmargin;
                } else {
                    if (row_L > -big_B && ninfU <= 1
                            && (ninfU == 0 || col_L < -big_B))
                        new_U = (row_L - xmaxU) / value + (1 - ninfU) * col_L
                                + xmaxUmargin;
                    if (row_U < +big_B && ninfL <= 1
                            && (ninfL == 0 || col_U > +big_B))
                        new_L = (row_U - xminL) / value + (1 - ninfL) * col_U
                                - xminLmargin;
                }
                if (new_U < col_U - 1.0e-12 && new_U < big_B) {
                    colUpper[iCol] = max(new_U, col_L);
                    numberChanged++;
                }
                if (new_L > col_L + 1.0e-12 && new_L > -big_B) {
                    colLower[iCol] = min(new_L, col_U);
                    numberChanged++;
                }
            }
        }
        if (numberChanged == 0)
            break;
        iPass++;
        if (iPass > 10)
            break;
    }
    double useTolerance = 1.0e-3;
    for (int iCol = 0; iCol < numCol; iCol++) {
        if (colUpper0[iCol] > colLower0[iCol] + useTolerance) {
            const double relax = 100.0 * useTolerance;
            if (colUpper[iCol] - colLower[iCol] < useTolerance + 1.0e-8) {
                colLower[iCol] = max(colLower0[iCol], colLower[iCol] - relax);
                colUpper[iCol] = min(colUpper0[iCol], colUpper[iCol] + relax);
            } else {
                if (colUpper[iCol] < colUpper0[iCol]) {
                    colUpper[iCol] = min(colUpper[iCol] + relax,
                            colUpper0[iCol]);
                }
                if (colLower[iCol] > colLower0[iCol]) {
                    colLower[iCol] = min(colLower[iCol] - relax,
                            colLower0[iCol]);
                }
            }
        }
    }
}
void HModel::setup_shuffleColumn() {
    if (intOption[INTOPT_PERMUTE_FLAG] == 0)
        return;
    // 1. Shuffle the column index
    HRandom localRandom;
    for (int i = 0; i < 10; i++)
        localRandom.intRandom();
    vector<int> iFrom(numCol);
    for (int i = 0; i < numCol; i++)
        iFrom[i] = i;
    for (int i = numCol - 1; i >= 1; i--) {
        int j = localRandom.intRandom() % (i + 1);
        swap(iFrom[i], iFrom[j]);
    }
    // 2. Save original copy
    vector<int> start = Astart;
    vector<int> index = Aindex;
    vector<double> value = Avalue;
    vector<double> lower = colLower;
    vector<double> upper = colUpper;
    vector<double> xcost = colCost;
    vector<int> ibreak = intBreak;
    vector<double> dxpert = dblXpert;
    // 3. Generate the permuted matrix
    int countX = 0;
    for (int i = 0; i < numCol; i++) {
        int ifrom = iFrom[i];
        Astart[i] = countX;
        for (int k = start[ifrom]; k < start[ifrom + 1]; k++) {
            Aindex[countX] = index[k];
            Avalue[countX] = value[k];
            countX++;
        }
        colLower[i] = lower[ifrom];
        colUpper[i] = upper[ifrom];
        colCost[i] = xcost[ifrom];
        intBreak[i] = ibreak[ifrom];
        dblXpert[i] = dxpert[ifrom];
    }
    assert(Astart[numCol] == countX);
}
void HModel::setup_allocWorking() {
    // Setup starting base
    basicIndex.resize(numRow);
    for (int iRow = 0; iRow < numRow; iRow++)
        basicIndex[iRow] = iRow + numCol;
    nonbasicFlag.assign(numTot, 0);
    nonbasicMove.resize(numTot);
    for (int i = 0; i < numCol; i++)
        nonbasicFlag[i] = 1;
    // Matrix, factor
    matrix.setup(numCol, numRow, &Astart[0], &Aindex[0], &Avalue[0]);
    factor.setup(numCol, numRow, &Astart[0], &Aindex[0], &Avalue[0],
            &basicIndex[0]);
    limitUpdate = 5000;
    // Setup other buffer
    buffer.setup(numRow);
    bufferLong.setup(numCol);
    // Setup bounds and solution spaces
    workCost.resize(numTot);
    workDual.resize(numTot);
    workShift.assign(numTot, 0);
    workLower.resize(numTot);
    workUpper.resize(numTot);
    workRange.resize(numTot);
    workValue.resize(numTot);
    baseLower.resize(numRow);
    baseUpper.resize(numRow);
    baseValue.resize(numRow);
}
void HModel::initCost(int perturb) {
    // Copy the cost
    for (int i = 0; i < numCol; i++)
        workCost[i] = colCost[i];
    for (int i = numCol; i < numTot; i++)
        workCost[i] = 0;
    workShift.assign(numTot, 0);
    // See if we want to skip perturbation
    problemPerturbed = 0;
    if (perturb == 0 || intOption[INTOPT_PERTURB_FLAG] == 0)
        return;
    problemPerturbed = 1;
    // Perturb the original costs, scale down if is too big
    double bigc = 0;
    for (int i = 0; i < numCol; i++)
        bigc = max(bigc, fabs(workCost[i]));
    if (bigc > 100)
        bigc = sqrt(sqrt(bigc));
    // If there's few boxed variables, we will just use Simple perturbation
    double boxedRate = 0;
    for (int i = 0; i < numTot; i++)
        boxedRate += (workRange[i] < 1e30);
    boxedRate /= numTot;
    if (boxedRate < 0.01)
        bigc = min(bigc, 1.0);
    if (bigc < 1) {
//        bigc = sqrt(bigc);
    }
    // Determine the perturbation base
    double base = 5e-7 * bigc;
    // Now do the perturbation
    for (int i = 0; i < numCol; i++) {
        double lower = colLower[i];
        double upper = colUpper[i];
        double xpert = (fabs(workCost[i]) + 1) * base * (1 + dblXpert[i]);
        if (lower == -HSOL_CONST_INF && upper == HSOL_CONST_INF) {
            // Free - no perturb
        } else if (upper == HSOL_CONST_INF) { // Lower
            workCost[i] += xpert;
        } else if (lower == -HSOL_CONST_INF) { // Upper
            workCost[i] += -xpert;
        } else if (lower != upper) { // Boxed
            workCost[i] += (workCost[i] >= 0) ? xpert : -xpert;
        } else {
            // Fixed - no perturb
        }
    }
    for (int i = numCol; i < numTot; i++) {
        workCost[i] += (0.5 - dblXpert[i]) * 1e-12;
    }
}
void HModel::initBound(int phase) {
    // Copy bounds
    for (int i = 0; i < numCol; i++) {
        workLower[i] = colLower[i];
        workUpper[i] = colUpper[i];
    }
    for (int i = 0, j = numCol; i < numRow; i++, j++) {
        workLower[j] = -rowUpper[i];
        workUpper[j] = -rowLower[i];
    }
    // Change to dual phase 1 bound
    if (phase == 1) {
        const double inf = HSOL_CONST_INF;
        for (int i = 0; i < numTot; i++) {
            if (workLower[i] == -inf && workUpper[i] == inf) {
                // Won't change for row variables: they should never
                // Become non basic
                if (i >= numCol)
                    continue;
                workLower[i] = -1000, workUpper[i] = 1000; // FREE
            } else if (workLower[i] == -inf) {
                workLower[i] = -1, workUpper[i] = 0; // UPPER
            } else if (workUpper[i] == inf) {
                workLower[i] = 0, workUpper[i] = 1; // LOWER
            } else {
                workLower[i] = 0, workUpper[i] = 0; // BOXED or FIXED
            }
        }
    }
    // Work out ranges
    for (int i = 0; i < numTot; i++)
        workRange[i] = workUpper[i] - workLower[i];
}
void HModel::initValue() {
    for (int i = 0; i < numTot; i++) {
        if (nonbasicFlag[i]) {
            if (workLower[i] == workUpper[i]) {
                // Fixed
                workValue[i] = workLower[i];
                nonbasicMove[i] = 0;
            } else if (workLower[i] != -HSOL_CONST_INF) {
                // Lower
                workValue[i] = workLower[i];
                nonbasicMove[i] = 1;
            } else if (workUpper[i] != +HSOL_CONST_INF) {
                // UPPER
                workValue[i] = workUpper[i];
                nonbasicMove[i] = -1;
            } else {
                // FREE
                workValue[i] = 0;
                nonbasicMove[i] = 0;
            }
        } else {
            nonbasicMove[i] = 0;
        }
    }
}
void HModel::computeFactor() {
    try {
        factor.build();
    } catch (runtime_error& error) {
      cerr << error.what() << endl;
        problemStatus = 3;
        // TODO Change to return
        exit(0);
    }
    countUpdate = 0;
}
void HModel::computeDual() {
    buffer.clear();
    for (int iRow = 0; iRow < numRow; iRow++) {
        buffer.index[iRow] = iRow;
        buffer.array[iRow] = workCost[basicIndex[iRow]]
                + workShift[basicIndex[iRow]];
    }
    buffer.count = numRow;
    factor.btran(buffer, 1);
    bufferLong.clear();
    matrix.price_by_col(bufferLong, buffer);
    for (int i = 0; i < numCol; i++)
        workDual[i] = workCost[i] - bufferLong.array[i];
    for (int i = numCol; i < numTot; i++)
        workDual[i] = workCost[i] - buffer.array[i - numCol];
}
void HModel::computeDualInfeasInDual(int *dualInfeasCount) {
    int workCount = 0;
    const double inf = HSOL_CONST_INF;
    const double tau_d = dblOption[DBLOPT_DUAL_TOL];
    for (int i = 0; i < numTot; i++) {
        // Only for non basic variables
        if (!nonbasicFlag[i])
            continue;
        // Free
        if (workLower[i] == -inf && workUpper[i] == inf)
            workCount += (fabs(workDual[i]) >= tau_d);
        // In dual, assuming that boxed variables will be flipped
        if (workLower[i] == -inf || workUpper[i] == inf)
            workCount += (nonbasicMove[i] * workDual[i] <= -tau_d);
    }
    *dualInfeasCount = workCount;
}
void HModel::computeDualInfeasInPrimal(int *dualInfeasCount) {
    int workCount = 0;
    const double inf = HSOL_CONST_INF;
    const double tau_d = dblOption[DBLOPT_DUAL_TOL];
    for (int i = 0; i < numTot; i++) {
        // Only for non basic variables
        if (!nonbasicFlag[i])
            continue;
        // Free
        if (workLower[i] == -inf && workUpper[i] == inf)
            workCount += (fabs(workDual[i]) >= tau_d);
        // In primal don't assume flip
        workCount += (nonbasicMove[i] * workDual[i] <= -tau_d);
    }
    *dualInfeasCount = workCount;
}
void HModel::correctDual(int *freeInfeasCount) {
    const double tau_d = dblOption[DBLOPT_DUAL_TOL];
    const double inf = HSOL_CONST_INF;
    int workCount = 0;
    for (int i = 0; i < numTot; i++) {
        if (nonbasicFlag[i]) {
            if (workLower[i] == -inf && workUpper[i] == inf) {
                // FREE variable
                workCount += (fabs(workDual[i]) >= tau_d);
            } else if (nonbasicMove[i] * workDual[i] <= -tau_d) {
                if (workLower[i] != -inf && workUpper[i] != inf) {
                    // Boxed variable = flip
                    flipBound(i);
                } else {
                    // Other variable = shift
                    problemPerturbed = 1;
                    if (nonbasicMove[i] == 1) {
                        double dual = (1 + random.dblRandom()) * tau_d;
                        double shift = dual - workDual[i];
                        workDual[i] = dual;
                        workCost[i] = workCost[i] + shift;
                    } else {
                        double dual = -(1 + random.dblRandom()) * tau_d;
                        double shift = dual - workDual[i];
                        workDual[i] = dual;
                        workCost[i] = workCost[i] + shift;
                    }
                }
            }
        }
    }
    *freeInfeasCount = workCount;
}
void HModel::computePrimal() {
    buffer.clear();
    for (int i = 0; i < numTot; i++)
        if (nonbasicFlag[i] && workValue[i] != 0)
            matrix.collect_aj(buffer, i, workValue[i]);
    factor.ftran(buffer, 1);
    for (int i = 0; i < numRow; i++) {
        int iCol = basicIndex[i];
        baseValue[i] = -buffer.array[i];
        baseLower[i] = workLower[iCol];
        baseUpper[i] = workUpper[iCol];
    }
}
void HModel::computeObject(int phase) {
    objective = 0;
    for (int i = 0; i < numTot; i++)
        if (nonbasicFlag[i])
            objective += workValue[i] * workDual[i];
    if (phase != 1)
        objective -= objOffset;
}
void HModel::shiftCost(int iCol, double amount) {
    problemPerturbed = 1;
    assert(workShift[iCol] == 0);
    workShift[iCol] = amount;
}
void HModel::shiftBack(int iCol) {
    workDual[iCol] -= workShift[iCol];
    workShift[iCol] = 0;
}
void HModel::flipBound(int iCol) {
    const int move = nonbasicMove[iCol] = -nonbasicMove[iCol];
    workValue[iCol] = move == 1 ? workLower[iCol] : workUpper[iCol];
}
void HModel::updateFactor(HVector *column, HVector *row_ep, int *iRow,
        int *hint) {
    factor.update(column, row_ep, iRow, hint);
    if (countUpdate >= limitUpdate)
        *hint = 1;
}
void HModel::updateMatrix(int columnIn, int columnOut) {
    matrix.update(columnIn, columnOut);
}
void HModel::updatePivots(int columnIn, int rowOut, int sourceOut) {
    int columnOut = basicIndex[rowOut];
    // Incoming variable
    basicIndex[rowOut] = columnIn;
    nonbasicFlag[columnIn] = 0;
    nonbasicMove[columnIn] = 0;
    baseLower[rowOut] = workLower[columnIn];
    baseUpper[rowOut] = workUpper[columnIn];
    // Outgoing variable
    nonbasicFlag[columnOut] = 1;
    if (workLower[columnOut] == workUpper[columnOut]) {
        workValue[columnOut] = workLower[columnOut];
        nonbasicMove[columnOut] = 0;
    } else if (sourceOut == -1) {
        workValue[columnOut] = workLower[columnOut];
        nonbasicMove[columnOut] = 1;
    } else {
        workValue[columnOut] = workUpper[columnOut];
        nonbasicMove[columnOut] = -1;
    }
    countUpdate++;
}
void HModel::changeUpdate(int updateMethod) {
    factor.change(updateMethod);
}
void HModel::reportPivots(int columnIn, int , double ) {
    if (columnIn >= 0)
      numberIteration++;
}
void HModel::reportStatus(int status) {
    problemStatus = status;
}
void HModel::printMessage(const char *message) {
    if (intOption[INTOPT_PRINT_FLAG])
        printf("%s\n", message);
}
void HModel::printObject() {
    if (intOption[INTOPT_PRINT_FLAG])
        printf("%10d  %20.10e\n", numberIteration, objective);
}
// #include <cstdio>
// #include <iostream>
using namespace std;
void HPrimal::solvePhase2(HModel *ptr_model) {
    // Copy size
    model = ptr_model;
    numCol = model->getNumCol();
    numRow = model->getNumRow();
    numTot = model->getNumTot();
    // Setup update limits
    limitUpdate = min(100 + numRow / 100, 1000);
    countUpdate = 0;
    // Setup local vectors
    column.setup(numRow);
    row_ep.setup(numRow);
    row_ap.setup(numCol);
    columnDensity = 0;
    row_epDensity = 0;
    // Setup other buffers
    model->printMessage("primal-start");
    for (;;) {
        primalRebuild();
        for (;;) {
            primalChooseColumn();
            if (columnIn == -1)
                break;
            primalChooseRow();
            if (rowOut == -1) {
                break;
            }
            primalUpdate();
            if (invertHint)
                break;
        }
        // Fresh factorization required
        if (countUpdate == 0)
            break;
    }
    if (columnIn == -1) {
        model->printMessage("primal-optimal");
        model->printMessage("problem-optimal");
        model->reportStatus(LP_Status_Optimal);
    } else {
        model->printMessage("primal-unbounded");
        model->reportStatus(LP_Status_Unbounded);
    }
}
void HPrimal::primalRebuild() {
    model->reportPivots(-1, -1, 0); // Indicate REINVERT
    // Rebuild model->factor - only if we got updates
    invertHint = 0;
    if (countUpdate > 0) {
        model->computeFactor();
        countUpdate = 0;
    }
    model->computeDual();
    model->computePrimal();
    model->computeObject();
    model->printObject();
}
void HPrimal::primalChooseColumn() {
    columnIn = -1;
    double bestInfeas = 0;
    const int *jFlag = model->getNonbasicFlag();
    const int *jMove = model->getNonbasicMove();
    double *workDual = model->getWorkDual();
    const double *workLower = model->getWorkLower();
    const double *workUpper = model->getWorkUpper();
    const double dualTolerance = model->dblOption[DBLOPT_DUAL_TOL];
    for (int iCol = 0; iCol < numTot; iCol++) {
        if (jFlag[iCol] && fabs(workDual[iCol]) > dualTolerance) {
            // Always take free
            // TODO: if we found free,
            // Then deal with it in dual phase 1
            if (workLower[iCol] == -HSOL_CONST_INF
                    && workUpper[iCol] == HSOL_CONST_INF) {
                columnIn = iCol;
                break;
            }
            // Then look at dual infeasible
            if (jMove[iCol] * workDual[iCol] < -dualTolerance) {
                if (bestInfeas < fabs(workDual[iCol])) {
                    bestInfeas = fabs(workDual[iCol]);
                    columnIn = iCol;
                }
            }
        }
    }
}
void HPrimal::primalChooseRow() {
    const double *baseLower = model->getBaseLower();
    const double *baseUpper = model->getBaseUpper();
    double *baseValue = model->getBaseValue();
    const double primalTolerance = model->dblOption[DBLOPT_PRIMAL_TOL];
    // Compute pivot column
    column.clear();
    column.packFlag = true;
    model->getMatrix()->collect_aj(column, columnIn, 1);
    model->getFactor()->ftran(column, columnDensity);
    columnDensity = 0.95 * columnDensity + 0.05 * column.count / numRow;
    // Initialize
    rowOut = -1;
    // Choose column pass 1
    double alphaTol = countUpdate < 10 ? 1e-9 : countUpdate < 20 ? 1e-8 : 1e-7;
    const int *jMove = model->getNonbasicMove();
    int moveIn = jMove[columnIn];
    if (moveIn == 0) {
        // If there's still free in the N
        // We would report not-solved
        // Need to handle free
    }
    double relaxTheta = 1e100;
    for (int i = 0; i < column.count; i++) {
        int index = column.index[i];
        double alpha = column.array[index] * moveIn;
        if (alpha > alphaTol) {
            double relaxSpace = baseValue[index] - baseLower[index]
                    + primalTolerance;
            if (relaxSpace < relaxTheta * alpha)
                relaxTheta = relaxSpace / alpha;
        } else if (alpha < -alphaTol) {
            double relaxSpace = baseValue[index] - baseUpper[index]
                    - primalTolerance;
            if (relaxSpace > relaxTheta * alpha)
                relaxTheta = relaxSpace / alpha;
        }
    }
    // Choose column pass 2
    double bestAlpha = 0;
    for (int i = 0; i < column.count; i++) {
        int index = column.index[i];
        double alpha = column.array[index] * moveIn;
        if (alpha > alphaTol) {
            double tightSpace = baseValue[index] - baseLower[index];
            if (tightSpace < relaxTheta * alpha) {
                if (bestAlpha < alpha) {
                    bestAlpha = alpha;
                    rowOut = index;
                }
            }
        } else if (alpha < -alphaTol) {
            double tightSpace = baseValue[index] - baseUpper[index];
            if (tightSpace > relaxTheta * alpha) {
                if (bestAlpha < -alpha) {
                    bestAlpha = -alpha;
                    rowOut = index;
                }
            }
        }
    }
}
void HPrimal::primalUpdate() {
    int *jMove = model->getNonbasicMove();
    double *workDual = model->getWorkDual();
    const double *workLower = model->getWorkLower();
    const double *workUpper = model->getWorkUpper();
    const double *baseLower = model->getBaseLower();
    const double *baseUpper = model->getBaseUpper();
    double *workValue = model->getWorkValue();
    double *baseValue = model->getBaseValue();
    const double primalTolerance = model->dblOption[DBLOPT_PRIMAL_TOL];
    // Compute thetaPrimal
    int moveIn = jMove[columnIn];
    int columnOut = model->getBaseIndex()[rowOut];
    double alpha = column.array[rowOut];
    double thetaPrimal = 0;
    if (alpha * moveIn > 0) {
        // Lower bound
        thetaPrimal = (baseValue[rowOut] - baseLower[rowOut]) / alpha;
    } else {
        // Upper bound
        thetaPrimal = (baseValue[rowOut] - baseUpper[rowOut]) / alpha;
    }
    // 1. Make sure it is inside bounds or just flip bound
    double lowerIn = workLower[columnIn];
    double upperIn = workUpper[columnIn];
    double valueIn = workValue[columnIn] + thetaPrimal;
    bool flipped = false;
    if (jMove[columnIn] == 1) {
        if (valueIn > upperIn + primalTolerance) {
            // Flip to upper
            workValue[columnIn] = upperIn;
            thetaPrimal = upperIn - lowerIn;
            flipped = true;
            jMove[columnIn] = -1;
        }
    } else if (jMove[columnIn] == -1) {
        if (valueIn < lowerIn - primalTolerance) {
            // Flip to lower
            workValue[columnIn] = lowerIn;
            thetaPrimal = lowerIn - upperIn;
            flipped = true;
            jMove[columnIn] = 1;
        }
    }
    for (int i = 0; i < column.count; i++) {
        int index = column.index[i];
        baseValue[index] -= thetaPrimal * column.array[index];
    }
    // If flipped, then no need touch the pivots
    if (flipped) {
        return;
    }
    // Pivot in
    int sourceOut = alpha * moveIn > 0 ? -1 : 1;
    model->updatePivots(columnIn, rowOut, sourceOut);
    baseValue[rowOut] = valueIn;
    // Check for any possible infeasible
    for (int iRow = 0; iRow < numRow; iRow++) {
        if (baseValue[iRow] < baseLower[iRow] - primalTolerance) {
            invertHint = 1;
        } else if (baseValue[iRow] > baseUpper[iRow] + primalTolerance) {
            invertHint = 1;
        }
    }
    // 2. Now we can update the dual
    row_ep.clear();
    row_ap.clear();
    row_ep.count = 1;
    row_ep.index[0] = rowOut;
    row_ep.array[rowOut] = 1;
    row_ep.packFlag = true;
    model->getFactor()->btran(row_ep, row_epDensity);
    model->getMatrix()->price_by_row(row_ap, row_ep);
    row_epDensity = 0.95 * row_epDensity + 0.05 * row_ep.count / numRow;
    double thetaDual = workDual[columnIn] / alpha;
    for (int i = 0; i < row_ap.count; i++) {
        int iCol = row_ap.index[i];
        workDual[iCol] -= thetaDual * row_ap.array[iCol];
    }
    for (int i = 0; i < row_ep.count; i++) {
        int iGet = row_ep.index[i];
        int iCol = iGet + numCol;
        workDual[iCol] -= thetaDual * row_ep.array[iGet];
    }
    // Dual for the pivot
    workDual[columnIn] = 0;
    workDual[columnOut] = -thetaDual;
    // Update model->factor basis
    model->updateFactor(&column, &row_ep, &rowOut, &invertHint);
    model->updateMatrix(columnIn, columnOut);
    if (++countUpdate >= limitUpdate)
        invertHint = true;
    model->reportPivots(columnIn, columnOut, alpha);
}
// #include <cmath>
// #include <cassert>
void HVector::setup(int size_) {
    size = size_;
    count = 0;
    index.resize(size);
    array.assign(size, 0);
    cwork.assign(size + 6400, 0); // MAX invert
    iwork.assign(size * 4, 0);
    packCount = 0;
    packIndex.resize(size);
    packValue.resize(size);
}
void HVector::clear() {
    int clearVector_inDense = count < 0 || count > size * 0.3;
    if (clearVector_inDense) {
        array.assign(size, 0);
    } else {
        for (int i = 0; i < count; i++)
            array[index[i]] = 0;
    }
    packFlag = false;
    count = 0;
    pseudoTick = 0;
    fakeTick = 0;
    next = 0;
}
void HVector::tight() {
    int totalCount = 0;
    for (int i = 0; i < count; i++) {
        const int my_index = index[i];
        const double value = array[my_index];
        if (fabs(value) > HSOL_CONST_TINY) {
            index[totalCount++] = my_index;
        } else {
            array[my_index] = 0;
        }
    }
    count = totalCount;
}
void HVector::pack() {
    if (packFlag) {
        packFlag = false;
        packCount = 0;
        for (int i = 0; i < count; i++) {
            const int ipack = index[i];
            packIndex[packCount] = ipack;
            packValue[packCount] = array[ipack];
            packCount++;
        }
    }
}
void HVector::copy(const HVector *from) {
    clear();
    fakeTick = from->fakeTick;
    pseudoTick = from->pseudoTick;
    const int fromCount = count = from->count;
    const int *fromIndex = &from->index[0];
    const double *fromArray = &from->array[0];
    for (int i = 0; i < fromCount; i++) {
        const int iFrom = fromIndex[i];
        const double xFrom = fromArray[iFrom];
        index[i] = iFrom;
        array[iFrom] = xFrom;
    }
}
double HVector::norm2() {
    const int workCount = count;
    const int *workIndex = &index[0];
    const double *workArray = &array[0];
    double result = 0;
    for (int i = 0; i < workCount; i++) {
        double value = workArray[workIndex[i]];
        result += value * value;
    }
    return result;
}
void HVector::saxpy(const double pivotX, const HVector *pivot) {
    int workCount = count;
    int *workIndex = &index[0];
    double *workArray = &array[0];
    const int pivotCount = pivot->count;
    const int *pivotIndex = &pivot->index[0];
    const double *pivotArray = &pivot->array[0];
    for (int k = 0; k < pivotCount; k++) {
        const int iRow = pivotIndex[k];
        const double x0 = workArray[iRow];
        const double x1 = x0 + pivotX * pivotArray[iRow];
        if (x0 == 0)
            workIndex[workCount++] = iRow;
        workArray[iRow] = (fabs(x1) < HSOL_CONST_TINY) ? HSOL_CONST_ZERO : x1;
    }
    count = workCount;
}

// #ifndef CP_GEN
// #define BACKWARD_HAS_BFD 1
// #include "backward.hpp"
// backward::SignalHandling sh;
// #endif
using namespace std;
// Macros
using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;
// Types
template<class T>
using min_queue = priority_queue<T, vector<T>, greater<T>>;
template<class T>
using max_queue = priority_queue<T>;
// Printing
template<class T>
void print_collection(ostream& out, T const& x);
template<class T, size_t... I>
void print_tuple(ostream& out, T const& a, index_sequence<I...>);
template<class... A>
ostream& operator<<(ostream& out, tuple<A...> const& x) {
  print_tuple(out, x, index_sequence_for<A...>{});
  return out;
}
template<class... A>
ostream& operator<<(ostream& out, pair<A...> const& x) {
  print_tuple(out, x, index_sequence_for<A...>{});
  return out;
}
template<class A, size_t N>
ostream& operator<<(ostream& out, array<A, N> const& x) { print_collection(out, x); return out; }
template<class A>
ostream& operator<<(ostream& out, vector<A> const& x) { print_collection(out, x); return out; }
template<class A>
ostream& operator<<(ostream& out, deque<A> const& x) { print_collection(out, x); return out; }
template<class A>
ostream& operator<<(ostream& out, multiset<A> const& x) { print_collection(out, x); return out; }
template<class A, class B>
ostream& operator<<(ostream& out, multimap<A, B> const& x) { print_collection(out, x); return out; }
template<class A>
ostream& operator<<(ostream& out, set<A> const& x) { print_collection(out, x); return out; }
template<class A, class B>
ostream& operator<<(ostream& out, map<A, B> const& x) { print_collection(out, x); return out; }
template<class A, class B>
ostream& operator<<(ostream& out, unordered_set<A> const& x) { print_collection(out, x); return out; }
template<class T, size_t... I>
void print_tuple(ostream& out, T const& a, index_sequence<I...>){
  using swallow = int[];
  out << '(';
  (void)swallow{0, (void(out << (I == 0? "" : ", ") << get<I>(a)), 0)...};
  out << ')';
}
template<class T>
void print_collection(ostream& out, T const& x) {
  int f = 0;
  out << '[';
  for(auto const& i: x) {
    out << (f++ ? "," : "");
    out << i;
  }
  out << "]";
}
// Random
struct RNG {
  u32 x, y, z, w;
  RNG(u64 seed) {
    reset(seed);
  }
  RNG() {
    reset(time(0));
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
    splitmix64_state s { seed };
    x = (u32)s.splitmix64();
    y = (u32)s.splitmix64();
    z = (u32)s.splitmix64();
    w = (u32)s.splitmix64();
  }
  inline u32 xorshift128() {
    u32 t = x;
    t ^= t << 11;
    t ^= t >> 8;
    x = y; y = z; z = w;
    w ^= w >> 19;
    w ^= t;
    return w;
  }
  // // UniformRandomBitGenerator interface
  // using result_type = u32;
  // constexpr u32 min(){ return numeric_limits<u32>::min(); }
  // constexpr u32 max(){ return numeric_limits<u32>::max(); }
  // u32 operator()() { return randomInt32(); }
  inline u32 randomInt32() {
    return xorshift128();
  }
  inline u64 randomInt64() {
    return (((u64)randomInt32())<<32ll) | ((u64)randomInt32());
  }
  inline u32 random32(u32 r) {
    return (((u64)randomInt32())*r)>>32;
  }
  inline u64 random64(u64 r) {
    return randomInt64()%r;
  }
  inline u32 randomRange32(u32 l, u32 r) {
    return l + random32(r-l+1);
  }
  inline u64 randomRange64(u64 l, u64 r) {
    return l + random64(r-l+1);
  }
  inline double randomDouble() {
    return (double)randomInt32() / 4294967296.0;
  }
  inline float randomFloat() {
    return (float)randomInt32() / 4294967296.0;
  }
  inline double randomRangeDouble(double l, double r) {
    return l + randomDouble() * (r-l);
  }
  template<class T>
  void shuffle(vector<T>& v) {
    i32 sz = v.size();
    for(i32 i = sz; i > 1; i--) {
      i32 p = random32(i);
      swap(v[i-1],v[p]);
    }
  }
  template<class T>
  void shuffle(T* fr, T* to) {
    i32 sz = distance(fr,to);
    for(int i = sz; i > 1; i--) {
      int p = random32(i);
      swap(fr[i-1],fr[p]);
    }
  }
  template<class T>
  inline int sample_index(vector<T> const& v) {
    return random32(v.size());
  }
  template<class T>
  inline T sample(vector<T> const& v) {
    return v[sample_index(v)];
  }
} rng;
// Letrec
template<class Fun>
class letrec_result {
  Fun fun_;
  public:
    template<class T>
    explicit letrec_result(T &&fun): fun_(forward<T>(fun)) {}
    template<class ...Args>
    decltype(auto) operator()(Args &&...args) {
      return fun_(ref(*this), forward<Args>(args)...);
    }
};
template<class Fun>
decltype(auto) letrec(Fun &&fun) {
  return letrec_result<decay_t<Fun>>(forward<Fun>(fun));
}
// Timer
struct timer {
  chrono::high_resolution_clock::time_point t_begin;
  timer() {
    t_begin = chrono::high_resolution_clock::now();
  }
  void reset() {
    t_begin = chrono::high_resolution_clock::now();
  }
  float elapsed() const {
    return chrono::duration<float>(chrono::high_resolution_clock::now() - t_begin).count();
  }
};
// Util
template<class T>
T& smin(T& x, T const& y) { x = min(x,y); return x; }
template<class T>
T& smax(T& x, T const& y) { x = max(x,y); return x; }
template<typename T>
int sgn(T val) {
  if(val < 0) return -1;
  if(val > 0) return 1;
  return 0;
}
static inline
string int_to_string(int val, int digits = 0) {
  string s = to_string(val);
  reverse(begin(s), end(s));
  while((int)s.size() < digits) s.push_back('0');
  reverse(begin(s), end(s));
  return s;
}
// Debug
static inline void debug_impl_seq() {
  cerr << "}";
}
template <class T, class... V>
void debug_impl_seq(T const& t, V const&... v) {
  cerr << t;
  if(sizeof...(v)) { cerr << ", "; }
  debug_impl_seq(v...);
}

const double p_alpha[25] =
  {1.90, 1.85, 1.8, 1.7, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, };
timer TM;
const i32 MAXN = 100;
const i32 MAXD = 25;
const i32 MAXQ = 3200;
using S = bitset<MAXN>;
i32 num_queries = 0;
i32 n,nd,nq;
namespace std {
  template<size_t N>
  bool operator<(const bitset<N>& x, const bitset<N>& y){
    for(i32 i = 0; i < (i32)(N); ++i) if(x[i] != y[i]) return y[i];
    return false;
  }
}
typedef vector<double> VD;
typedef vector<VD> VVD;
typedef vector<int> VI;
const double EPS = 1e-9;
map<tuple<S,S>, i32> cache;
i32 hits = 0;
i32 dfs_hits = 0;
i32 lp_hits = 0;
vector<vector<tuple<i32, double>>> A;
VD B;
vector<vector<double>> XS;
HModel model;
bool lp_enabled = 0;
float solveLP(VD const& C) {
  if(B.empty()) return 1;
  model.setupstart();
  model.numCol = C.size();
  model.numRow = B.size();
  model.colCost.resize(model.numCol);
  model.colLower.resize(model.numCol);
  model.colUpper.resize(model.numCol);
  for(i32 i = 0; i < (i32)(C.size()); ++i) {
    model.colCost[i] = -C[i];
    model.colLower[i] = 0;
    model.colUpper[i] = 1e6;
  }
  model.rowLower.resize(model.numRow);
  model.rowUpper.resize(model.numRow);
  for(i32 i = 0; i < (i32)(B.size()); ++i) {
    model.rowLower[i] = -HSOL_CONST_INF;
    model.rowUpper[i] = B[i];
  }
  model.Astart.clear();
  model.Aindex.clear();
  model.Avalue.clear();
  for(i32 j = 0; j < (i32)(C.size()); ++j) {
    model.Astart.push_back(model.Aindex.size());
    for(i32 i = 0; i < (i32)(A[j].size()); ++i) {
      model.Aindex.push_back(get<0>(A[j][i]));
      model.Avalue.push_back(get<1>(A[j][i]));
    }
  }
  model.Astart.push_back(model.Aindex.size());
  model.objOffset = 0;
  model.setupend();
  HDual solver;
  solver.solve(&model);
  if(model.problemStatus == -1) return 1;
  if(model.problemStatus != 0) {
    do { cerr << "main.cpp" ":" "86" "  {" << "\"BAD\"" << "} = {"; debug_impl_seq("BAD"); cerr << endl << flush; } while(0);
    exit(0);
  }
  XS.emplace_back();
  auto& X = XS.back();
  X = model.workValue;
  for(i32 i = 0; i < (i32)(model.numRow); ++i) X[model.basicIndex[i]] = model.baseValue[i];
  X.resize(model.numCol);
  return -model.objective;
}
map<S, i32> M;
vector<vector<i32>> G;
vector<i32> last;
i32 date = 0;
vector<i32> Q;
i32 getSidx(S x) {
  auto it = M.find(x);
  if(it != end(M)) return it->second;
  i32 ix = M.size();
  M[x] = ix;
  G.emplace_back();
  last.emplace_back(0);
  return ix;
}
bool dfs(i32 u, i32 v){
  date += 1;
  Q.clear();
  Q.emplace_back(u); last[u] = date;
  while(!Q.empty()) {
    i32 w = Q.back(); Q.pop_back();
    if(w == v) return 1;
    for(auto z : G[w]) if(last[z] != date) {
        last[z] = date;
        Q.emplace_back(z);
    }
  }
  return false;
}
i64 tot_queries = 0;
i32 query(S s1, S s2) {
  tot_queries += 1;
  auto s = (s1&s2);
  s1 ^= s;
  s2 ^= s;
  if(s1 == s2) return 0;
  if(s1.none()) return -1;
  if(s2.none()) return 1;
  i32 mult = 1;
  if(s2 < s1) {
    swap(s1,s2);
    mult = -1;
  }
  { auto it = cache.find(make_tuple(s1,s2));
    if(it != cache.end()) {
      hits += 1;
      return it->second * mult;
    }
  }
  auto is1 = getSidx(s1);
  auto is2 = getSidx(s2);
  i32 res = 0;
  if(TM.elapsed() < TL){
    bool d1 = dfs(is1,is2);
    bool d2 = dfs(is2,is1);
    // if(d1 && d2) return 0;
    if(d1&&!d2) { res = -1; dfs_hits += 1; }
    if(d2&&!d1) { res = 1; dfs_hits += 1; }
  }
  try {
    if(lp_enabled && TM.elapsed() < TL_SIMPLEX){
      bool can1 = 1, can2 = 1;
      for(auto const& X : XS) {
        double v = 0;
        for(i32 i = 0; i < (i32)(n); ++i) if(s1[i]) v -= X[i];
        for(i32 i = 0; i < (i32)(n); ++i) if(s2[i]) v += X[i];
        if(v > 0) can1 = 0;
        if(v < 0) can2 = 0;
        if(!can1 && !can2) break;
      }
      if(res == 0 && can1) {
        VD C(n,0);
        for(i32 i = 0; i < (i32)(n); ++i) if(s1[i]) C[i] = -1;
        for(i32 i = 0; i < (i32)(n); ++i) if(s2[i]) C[i] = 1;
        auto y = solveLP(C);
        if(y < 0) {
          lp_hits += 1;
          res = 1;
        }
      }
      if(res == 0 && can2){
        VD C(n,0);
        for(i32 i = 0; i < (i32)(n); ++i) if(s1[i]) C[i] = 1;
        for(i32 i = 0; i < (i32)(n); ++i) if(s2[i]) C[i] = -1;
        auto y = solveLP(C);
        if(y < 0) {
          lp_hits += 1;
          res = -1;
        }
      }
    }
  }catch(int){}
  if(res != 0) {
    cache[make_tuple(s1,s2)] = res;
    if(res == -1) G[is1].emplace_back(is2);
    else if(res == 1) G[is2].emplace_back(is1);
    return res * mult;
  }
  if(num_queries == nq) {
    // int* x = 0; cout << *x << endl;
    // debug("fail query");
    return 0;
  }
  if(res == 0) {
    num_queries += 1;
    cout << s1.count() << " " << s2.count();
    for(i32 i = 0; i < (i32)(MAXN); ++i) if(s1[i]) cout << " " << i;
    for(i32 i = 0; i < (i32)(MAXN); ++i) if(s2[i]) cout << " " << i;
    cout << endl;
    char c; cin>>c;
    if(c == '<') res = -1;
    else if(c == '=') res = 0;
    else res = 1;
  }
  if(res == 0){
    XS.clear();
    { for(i32 i = 0; i < (i32)(n); ++i) if(s1[i]) A[i].emplace_back(B.size(),1);
      for(i32 i = 0; i < (i32)(n); ++i) if(s2[i]) A[i].emplace_back(B.size(),-1);
      B.emplace_back(0.001);
    }
    { for(i32 i = 0; i < (i32)(n); ++i) if(s1[i]) A[i].emplace_back(B.size(),-1);
      for(i32 i = 0; i < (i32)(n); ++i) if(s2[i]) A[i].emplace_back(B.size(),1);
      B.emplace_back(0.001);
    }
  }else if(res == -1) {
    { vector<vector<double>> YS;
      for(auto& X : XS) {
        double v = 0;
        for(i32 i = 0; i < (i32)(n); ++i) if(s1[i]) v -= X[i];
        for(i32 i = 0; i < (i32)(n); ++i) if(s2[i]) v += X[i];
        if(v <= -1) YS.emplace_back(std::move(X));
      }
      XS = std::move(YS);
    }
    for(i32 i = 0; i < (i32)(n); ++i) if(s1[i]) A[i].emplace_back(B.size(),1);
    for(i32 i = 0; i < (i32)(n); ++i) if(s2[i]) A[i].emplace_back(B.size(),-1);
    B.emplace_back(-1);
  }else if(res == 1) {
    { vector<vector<double>> YS;
      for(auto& X : XS) {
        double v = 0;
        for(i32 i = 0; i < (i32)(n); ++i) if(s1[i]) v -= X[i];
        for(i32 i = 0; i < (i32)(n); ++i) if(s2[i]) v += X[i];
        if(v >= 1) YS.emplace_back(std::move(X));
      }
      XS = std::move(YS);
    }
    for(i32 i = 0; i < (i32)(n); ++i) if(s1[i]) A[i].emplace_back(B.size(),-1);
    for(i32 i = 0; i < (i32)(n); ++i) if(s2[i]) A[i].emplace_back(B.size(),1);
    B.emplace_back(-1);
  }
  cache[make_tuple(s1,s2)] = res;
  if(res <= 0) G[is1].emplace_back(is2);
  if(res >= 0) G[is2].emplace_back(is1);
  return res * mult;
}
inline
S singleton(i32 i) {
  S s = 0; s[i] = 1;
  return s;
}
float ql;
void read() {
  cin>>n>>nd>>nq;
  cerr << "[DATA] n = " << n << endl;
  cerr << "[DATA] d = " << nd << endl;
  cerr << "[DATA] q = " << nq << endl;
  ql = log2(nq/n);
  cerr << "[DATA] ql = " << (int)(10*log2((float)nq/n)) << endl;
}
vector<i32> merge_insertion(vector<S> I) {
  i32 n = I.size();
  if(n == 1) return {0};
  vector<i32> L((n+1)/2);
  vector<i32> R(n/2);
  for(i32 i = 0; i < (i32)(n); ++i) {
    if(i&1) R[i/2] = i;
    else L[i/2] = i;
  }
  for(i32 i = 0; i < (i32)(n/2); ++i) {
    if(query(I[L[i]], I[R[i]]) > 0) {
      swap(L[i],R[i]);
    }
  }
  vector<S> IR(n/2); for(i32 i = 0; i < (i32)(n/2); ++i) IR[i] = I[R[i]];
  auto J = merge_insertion(IR);
  vector<i32> X;
  vector<tuple<i32,i32>> Y;
  X.emplace_back(L[J[0]]);
  for(i32 i = 0; i < (i32)(n/2); ++i) X.emplace_back(R[J[i]]);
  for(i32 i = 0; i < (i32)(n/2); ++i) if(i != 0) Y.emplace_back(L[J[i]],R[J[i]]);
  if(n&1) Y.emplace_back(L[n/2], -1);
  auto insert = [&](i32 i, i32 j) {
    i32 lo = 0, hi = X.size();
    for(i32 k = 0; k < (i32)(X.size()); ++k) if(X[k] == j) { hi = k; }
    for(i32 k = (8); k >= (i32)(0); --k) if(hi-lo >= (1<<k)) {
      i32 mi = hi - (1<<k); // lo + (1<<k) - 1;
      auto q = query(I[i], I[X[mi]]);
      if(q == 0) {
        lo = hi = mi;
      }else if(q < 0) {
        hi = mi;
      }else{
        lo = mi+1;
      }
    }
    // while(lo != hi) {
    //   i32 mi = lo + (hi-lo)/2;
    //   auto res = query(I[i], I[X[mi]]);
    //   if(res == 0) {
    //     lo = hi = mi;
    //   }else if(res < 0) {
    //     hi = mi;
    //   }else{
    //     lo = mi+1;
    //   }
    // }
    X.insert(begin(X)+lo,i);
  };
  { i32 sz1 = 2;
    i32 sz2 = 2;
    i32 at = 0;
    while(at < (i32)Y.size()) {
      i32 to = min(at+sz1, (i32)Y.size());
      reverse(begin(Y)+at, begin(Y)+to);
      at += sz1;
      i32 sz3 = 2*sz1+sz2;
      sz1 = sz2;
      sz2 = sz3;
    }
  }
  for(auto p : Y) insert(get<0>(p),get<1>(p));
  return X;
}
vector<S> merge_insertion_sort(vector<S> A) {
  auto I = merge_insertion(A);
  vector<S> B(A.size());
  for(i32 i = 0; i < (i32)(A.size()); ++i) B[i] = A[I[i]];
  return B;
}
i32 query_kk(array<S,2> a, array<S,2> b) {
  return query(a[0]^b[1], a[1]^b[0]);
}
vector<i32> merge_insertion_kk(vector<array<S,2>> I) {
  i32 n = I.size();
  if(n == 1) return {0};
  vector<i32> L((n+1)/2);
  vector<i32> R(n/2);
  for(i32 i = 0; i < (i32)(n); ++i) {
    if(i&1) R[i/2] = i;
    else L[i/2] = i;
  }
  for(i32 i = 0; i < (i32)(n/2); ++i) {
    if(query_kk(I[L[i]], I[R[i]]) > 0) {
      swap(L[i],R[i]);
    }
  }
  vector<array<S,2>> IR(n/2); for(i32 i = 0; i < (i32)(n/2); ++i) IR[i] = I[R[i]];
  auto J = merge_insertion_kk(IR);
  vector<i32> X;
  vector<tuple<i32,i32>> Y;
  X.emplace_back(L[J[0]]);
  for(i32 i = 0; i < (i32)(n/2); ++i) X.emplace_back(R[J[i]]);
  for(i32 i = 0; i < (i32)(n/2); ++i) if(i != 0) Y.emplace_back(L[J[i]],R[J[i]]);
  if(n&1) Y.emplace_back(L[n/2], -1);
  auto insert = [&](i32 i, i32 j) {
    i32 lo = 0, hi = X.size();
    for(i32 k = 0; k < (i32)(X.size()); ++k) if(X[k] == j) { hi = k; }
    for(i32 k = (8); k >= (i32)(0); --k) if(hi-lo >= (1<<k)) {
      i32 mi = hi - (1<<k); // lo + (1<<k) - 1;
      auto q = query_kk(I[i], I[X[mi]]);
      if(q == 0) {
        lo = hi = mi;
      }else if(q < 0) {
        hi = mi;
      }else{
        lo = mi+1;
      }
    }
    X.insert(begin(X)+lo,i);
  };
  { i32 sz1 = 2;
    i32 sz2 = 2;
    i32 at = 0;
    while(at < (i32)Y.size()) {
      i32 to = min(at+sz1, (i32)Y.size());
      reverse(begin(Y)+at, begin(Y)+to);
      at += sz1;
      i32 sz3 = 2*sz1+sz2;
      sz1 = sz2;
      sz2 = sz3;
    }
  }
  for(auto p : Y) insert(get<0>(p),get<1>(p));
  return X;
}
vector<array<S,2>> merge_insertion_sort_kk(vector<array<S,2>> A) {
  auto I = merge_insertion_kk(A);
  vector<array<S,2>> B(A.size());
  for(i32 i = 0; i < (i32)(A.size()); ++i) B[i] = A[I[i]];
  return B;
}
const i32 K2 = 8;
vector<S> insert_increasing(vector<S> A, S key) {
  i32 lo = 0, hi = A.size();
  while(lo != hi) {
    i32 mi = (lo+hi)/2;
    i32 q = query(key ,A[mi]);
    if(q == 0) {
      lo = hi = mi;
    }else if(q<0) {
      hi = mi;
    }else{
      lo=mi+1;
    }
  }
  A.insert(begin(A)+lo,key);
  return A;
}
vector<S> merge_sorted(vector<S> a, vector<S> b) {
  i32 n = a.size(), m = b.size();
  i32 i = 0, j = 0;
  vector<S> c;
  while(i < n && j < m) {
    auto q = query(a[i],b[j]);
    if(q == 0) {
      c.emplace_back(a[i]);
      i++;
      j++;
    }else if(q < 0) {
      c.emplace_back(a[i]);
      i++;
    }else{
      c.emplace_back(b[j]);
      j++;
    }
  }
  while(i < n) { c.emplace_back(a[i]); i++; }
  while(j < m) { c.emplace_back(b[j]); j++; }
  return c;
}
vector<S> sort_subsets(vector<S> a) {
  vector<S> cur = {0};
  for(auto x : a) {
    vector<S> l = cur;
    vector<S> r = cur;
    for(auto& k : r) k |= x;
    cur = merge_sorted(l,r);
  }
  return cur;
}
vector<array<S,2>> insert_ckk(vector<array<S,2>> A, array<S,2> key) {
  i32 lo = 0, hi = A.size();
  for(i32 k = (8); k >= (i32)(0); --k) if(hi-lo >= 1<<k){
    i32 mi = lo+(hi-lo)/2;
    i32 q = query(key[0]^A[mi][1], key[1]^A[mi][0]);
    if(q == 0) {
      lo = hi = mi;
    }else if(q>0) {
      hi = mi;
    }else{
      lo=mi+1;
    }
  }
  A.insert(begin(A)+lo,key);
  return A;
}
array<S,2> run_ckk1(vector<array<S,2>> const& L, array<S,2> best, bool stop_on_best = false) {
  do { cerr << "main.cpp" ":" "517" "  {" << "\"start CKK\", num_queries, nq" << "} = {"; debug_impl_seq("start CKK", num_queries, nq); cerr << endl << flush; } while(0);
  bool should_stop = false;
  auto backtrack = letrec([&](auto backtrack, vector<array<S,2>> A) -> void {
    if(nd != 2 && num_queries + 2 >= nq) {
      should_stop = 1;
      return;
    }
    if(A.size() == 1) {
      auto a = A.back();
      auto q = query(best[0],a[0]);
      if(q<0) {
        do { cerr << "main.cpp" ":" "528" "  {" << "\"CKK new best\", num_queries" << "} = {"; debug_impl_seq("CKK new best", num_queries); cerr << endl << flush; } while(0);
        best = a;
        if(stop_on_best) { should_stop = 1; }
      }
      if(q == 0 && nd > 5 && stop_on_best) { should_stop = 1; }
      if(num_queries >= nq) { should_stop = 1; }
    }else{
      auto a = A.back(); A.pop_back();
      auto b = A.back(); A.pop_back();
      { array<S,2> c; c[0] = a[0]|b[1]; c[1] = a[1]|b[0];
        if(query(c[1],c[0]) < 0) swap(c[0],c[1]);
        backtrack(insert_ckk(A, c));
      }
      if(should_stop) return;
      { array<S,2> c; c[0] = a[0]|b[0]; c[1] = a[1]|b[1];
        backtrack(insert_ckk(A, c));
      }
    }
  });
  backtrack(L);
  return best;
}
array<S,2> run_ckk(vector<S> const& s, array<S,2> best, bool stop_on_best = false) {
  vector<array<S,2>> L(s.size(), {0,0} );
  for(i32 i = 0; i < (i32)(s.size()); ++i) L[i][1] = s[i];
  return run_ckk1(L,best,stop_on_best);
}
array<S,2> solve_ss(vector<S> s, array<S,2> best) {
  do { cerr << "main.cpp" ":" "558" "  {" << "\"solve ss\"" << "} = {"; debug_impl_seq("solve ss"); cerr << endl << flush; } while(0);
  auto ALL = best[0]|best[1];
  vector<S> sa, sb;
  while(s.size() > 1) {
    auto x = s.back(); s.pop_back();
    if(sa.size() < sb.size()) sa.emplace_back(x);
    else sb.emplace_back(x);
  }
  auto ssa = sort_subsets(sa);
  auto ssb = sort_subsets(sb);
  i32 i = 0, j = ssb.size()-1;
  while(i < (int)ssa.size() && j >= 0) {
    if(num_queries+2 >= nq) break;
    auto x = s[0]|ssa[i]|ssb[j];
    auto y = x^ALL;
    auto q = query(x,y);
    if(q == 0) {
      best[0] = x;
      best[1] = y;
      return best;
    }
    if(q > 0) swap(x,y);
    if(query(best[0], x) < 0) {
      best[0] = x;
      best[1] = y;
    }
    if(q < 0) i += 1;
    else j -= 1;
  }
  return best;
}
void solve_smalld(i32 ngroup){
  S ALL = 0;
  for(i32 i = 0; i < (i32)(n); ++i) ALL[i] = 1;
  vector<array<S,2>> R;
  { vector<i32> A(n);
    for(i32 i = 0; i < (i32)(n); ++i) A[i] = i%ngroup;
    rng.shuffle(A);
    vector<array<S,2>> I(ngroup);
    for(i32 i = 0; i < (i32)(n); ++i) {
      auto& s = I[A[i]];
      if(s[0].count() < s[1].count()) s[0][i]=1;
      else s[1][i]=1;
    }
    for(i32 i = 0; i < (i32)(ngroup); ++i) {
      if(query(I[i][0],I[i][1])>0) {
        swap(I[i][0],I[i][1]);
      }
    }
    R = merge_insertion_sort_kk(I);
  }
  do { cerr << "main.cpp" ":" "611" "  {" << "\"after sort\", 160" << "} = {"; debug_impl_seq("after sort", 160); cerr << endl << flush; } while(0);
  lp_enabled = 1;
  reverse(begin(R), end(R));
  vector<S> sol(nd);
  if(nd == 2) {
    auto best = run_ckk1(R, {0,ALL});
    sol[0] = best[0];
    sol[1] = best[1];
  }else {
    vector<vector<S>> solkk(ngroup, vector<S>(nd));
    for(i32 i = 0; i < (i32)(ngroup); ++i) {
      solkk[i][nd-1] = R[i][1];
      solkk[i][nd-2] = R[i][0];
    }
    auto insert = [&](vector<S> key) {
      i32 lo = 0, hi = solkk.size();
      while(lo != hi) {
        i32 mi = (lo+hi)/2;
        auto q = query(key[nd-1] ^ solkk[mi][0], key[0] ^ solkk[mi][nd-1]);
        if(q == 0) {
          lo = hi = mi;
        }else if(q < 0) {
          hi = mi;
        }else{
          lo = mi+1;
        }
      }
      solkk.insert(begin(solkk)+lo, key);
    };
    reverse(begin(solkk), end(solkk));
    while(solkk.size() > 1) {
      do { cerr << "main.cpp" ":" "645" "  {" << "solkk.size(), num_queries" << "} = {"; debug_impl_seq(solkk.size(), num_queries); cerr << endl << flush; } while(0);
      auto a = solkk.back(); solkk.pop_back();
      auto b = solkk.back(); solkk.pop_back();
      vector<S> c(nd);
      for(i32 i = 0; i < (i32)(nd); ++i) c[i] = a[i]^b[nd-1-i];
      insert(merge_insertion_sort(c));
    }
    sol = solkk[0];
    reverse(begin(sol), end(sol));
  }
  do { cerr << "main.cpp" ":" "657" "  {" << "num_queries,nq" << "} = {"; debug_impl_seq(num_queries,nq); cerr << endl << flush; } while(0);
  while(num_queries < nq) {
    num_queries += 1;
    cout << "1 1 0 1" << endl;
    char c; cin>>c;
  }
  { vector<i32> col(n,0);
    for(i32 i = 0; i < (i32)(nd); ++i) for(i32 j = 0; j < (i32)(n); ++j) if(sol[i][j]) col[j] = i;
    for(i32 i = 0; i < (i32)(n); ++i) cout << col[i] << ' ';
    cout << endl;
  }
}
void solve(){
  S ALL = 0;
  for(i32 i = 0; i < (i32)(n); ++i) ALL[i] = 1;
  i32 ngroup = n;
  double p = 1.0;
  if(nd == 2) p = 0.99;
  if(nd == 3) p = 1.02;
  if(nd == 4) p = 1.02;
  if(nd == 5) p = 1.01;
  if(nd == 6) p = 1.01;
  if(nd == 7) p = 1.02;
  if(nd >= 15) p = 1.0;
  if(nd >= 19) p = 0.99;
  if(nd >= 21) p = 0.97;
  while(p_alpha[nd-2] * pow(ngroup, p) * log2(ngroup) > nq) ngroup -= 1;
  do { cerr << "main.cpp" ":" "689" "  {" << "ngroup" << "} = {"; debug_impl_seq(ngroup); cerr << endl << flush; } while(0);
  if(nd <= 2) {
    solve_smalld(ngroup);
    return;
  }
  vector<S> R(ngroup);
  { vector<i32> A(n);
    for(i32 i = 0; i < (i32)(n); ++i) A[i] = i%ngroup;
    rng.shuffle(A);
    vector<S> I(ngroup); for(i32 i = 0; i < (i32)(n); ++i) I[A[i]][i] = 1;
    rng.shuffle(I);
    auto J = merge_insertion(I);
    for(i32 i = 0; i < (i32)(ngroup); ++i) R[i] = I[J[i]];
  }
  auto insertR = [&](S key) {
    i32 lo = 0, hi = R.size();
    while(lo != hi) {
      auto mi = lo + (hi-lo)/2;
      if(query(key, R[mi]) < 0) {
        hi = mi;
      }else{
        lo = mi+1;
      }
    }
    R.insert(begin(R)+lo, key);
  };
  lp_enabled = 1;
  do { cerr << "main.cpp" ":" "720" "  {" << "\"after sort\", num_queries" << "} = {"; debug_impl_seq("after sort", num_queries); cerr << endl << flush; } while(0);
  i32 num_split = 0;
  if(nd <= 13) num_split = 1;
  else num_split = 2;
  while(ngroup < n && num_split > 0) {
    bool imp = 0;
    for(i32 i = (ngroup-1); i >= (i32)(0); --i) {
      auto s = R[i];
      if(s.count() == 1) continue;
      do { cerr << "main.cpp" ":" "731" "  {" << "num_queries + log2(ngroup) * s.count()" << "} = {"; debug_impl_seq(num_queries + log2(ngroup) * s.count()); cerr << endl << flush; } while(0);
      do { cerr << "main.cpp" ":" "734" "  {" << "num_queries + log2(ngroup) * s.count() + (ngroup+s.count()-nd) * log2(nd)" << "} = {"; debug_impl_seq(num_queries + log2(ngroup) * s.count() + (ngroup+s.count()-nd) * log2(nd)); cerr << endl << flush; } while(0);
      if(num_queries
         + log2(ngroup) * s.count()
         + (ngroup+s.count()-nd) * log2(nd)
         > nq) break;
      R.erase(begin(R)+i);
      for(i32 i = 0; i < (i32)(n); ++i) if(s[i]) insertR(singleton(i));
      ngroup = R.size();
      do { cerr << "main.cpp" ":" "742" "  {" << "\"SPLIT LARGE\", num_queries" << "} = {"; debug_impl_seq("SPLIT LARGE", num_queries); cerr << endl << flush; } while(0);
      num_split -= 1;
      imp = 1;
      break;
    }
    if(!imp) break;
  }
  do { cerr << "main.cpp" ":" "750" "  {" << "ngroup" << "} = {"; debug_impl_seq(ngroup); cerr << endl << flush; } while(0);
  vector<S> sol(nd, 0);
  do { cerr << "main.cpp" ":" "754" "  {" << "\"after split\", num_queries" << "} = {"; debug_impl_seq("after split", num_queries); cerr << endl << flush; } while(0);
  map<int,int> insert_pos;
  i32 cost_insert = 0;
  auto insert = [&](S key) {
    i32 lo = 0, hi = sol.size();
    for(i32 k = (8); k >= (i32)(0); --k) if(hi-lo >= (1<<k)) {
      i32 mi = hi - (1<<k);
      if(num_queries == nq) {
        lo = hi;
        break;
      }
      cost_insert += 1;
      auto q = query(key, sol[mi]);
      if(q == 0) {
        lo = hi = mi;
      }else if(q > 0) {
        hi = mi;
      }else{
        lo = mi+1;
      }
    }
    insert_pos[lo] += 1;
    sol.insert(begin(sol)+lo, key);
  };
  if(nd == 2) {
    auto best = run_ckk(R, {0,ALL});
    sol = {best[0],best[1]};
    reverse(begin(sol), end(sol));
    reverse(begin(R), end(R));
  }else if(nd <= 5) {
    vector<vector<S>> solkk(ngroup, vector<S>(nd));
    for(i32 i = 0; i < (i32)(ngroup); ++i) solkk[i][nd-1] = R[i];
    auto insert = [&](vector<S> key) {
      i32 lo = 0, hi = solkk.size();
      while(lo != hi) {
        i32 mi = (lo+hi)/2;
        auto q = query(key[nd-1] ^ solkk[mi][0], key[0] ^ solkk[mi][nd-1]);
        if(q == 0) {
          lo = hi = mi;
        }else if(q < 0) {
          hi = mi;
        }else{
          lo = mi+1;
        }
      }
      solkk.insert(begin(solkk)+lo, key);
    };
    while(solkk.size() > 1) {
      do { cerr << "main.cpp" ":" "807" "  {" << "solkk.size(), num_queries" << "} = {"; debug_impl_seq(solkk.size(), num_queries); cerr << endl << flush; } while(0);
      auto a = solkk.back(); solkk.pop_back();
      auto b = solkk.back(); solkk.pop_back();
      vector<S> c(nd);
      for(i32 i = 0; i < (i32)(nd); ++i) c[i] = a[i]^b[nd-1-i];
      insert(merge_insertion_sort(c));
    }
    sol = solkk[0];
    reverse(begin(sol), end(sol));
    reverse(begin(R), end(R));
  }else{
    reverse(begin(R), end(R));
    for(S ss : R) {
      auto s = sol.back();
      sol.pop_back();
      s |= ss;
      insert(s);
    }
  }
  do { cerr << "main.cpp" ":" "828" "  {" << "\"after kk/greedy\", num_queries" << "} = {"; debug_impl_seq("after kk/greedy", num_queries); cerr << endl << flush; } while(0);
  do { cerr << "main.cpp" ":" "830" "  {" << "insert_pos" << "} = {"; debug_impl_seq(insert_pos); cerr << endl << flush; } while(0);
  do { cerr << "main.cpp" ":" "831" "  {" << "cost_insert" << "} = {"; debug_impl_seq(cost_insert); cerr << endl << flush; } while(0);
  reverse(begin(R), end(R));
  num_split = 1;
  if(ql >= 2.5) num_split = 2;
  while(nd > 5 && num_queries < nq && num_split > 0) {
    {
      auto i1 = 0;
      auto i2 = nd-1;
      auto s1 = sol[i1];
      auto s2 = sol[i2];
      for(i32 i = (ngroup-1); i >= (i32)(0); --i) if((s1&R[i]).any() && R[i].count() > 1) {
        for(i32 j = 0; j < (i32)(n); ++j) if(R[i][j]) {
          auto x = singleton(j);
          if(num_queries+1 < nq && query(s2^x, s1) < 0) {
            do { cerr << "main.cpp" ":" "846" "  {" << "\"HERE\", num_queries, nq" << "} = {"; debug_impl_seq("HERE", num_queries, nq); cerr << endl << flush; } while(0);
            auto y = R[i]^x;
            sol.erase(begin(sol)+i2);
            sol.erase(begin(sol)+i1);
            insert(s2^x);
            insert(s1^x);
            R.erase(begin(R)+i);
            insertR(x);
            insertR(y);
            ngroup = R.size();
            num_split -= 1;
            goto lend1;
          }
        }
      }
    }
    break;
  lend1:;
    // TODO run num split 2
  }
  reverse(begin(R), end(R));
  static bool dp1[MAXN][MAXN];
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(n); ++j) dp1[i][j] = 0;
  auto setdp1 = letrec([&](auto setdp1, i32 i, i32 j) {
    if(dp1[i][j]) return;
    dp1[i][j] = 1;
    if(j+1 < ngroup) setdp1(i,j+1);
    if(i > 0) setdp1(i-1,j);
  });
  static bitset<MAXN> dp21[MAXN][MAXN][MAXN];
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(n); ++j) for(i32 k = 0; k < (i32)(n); ++k) dp21[i][j][k] = 0;
  auto setdp21 = letrec([&](auto setdp21, i32 i, i32 j, i32 k, i32 l) -> void {
    if(dp21[i][j][k][l]) return;
    dp21[i][j][k][l] = 1;
    if(i > 0) setdp21(i-1,j,k,l);
    if(j+1 < ngroup) setdp21(i,j+1,k,l);
    if(k+1 < ngroup) setdp21(i,j,k+1,l);
    if(l > 0) setdp21(i,j,k,l-1);
  });
  static bitset<MAXN> dp22[MAXN][MAXN][MAXN];
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(n); ++j) for(i32 k = 0; k < (i32)(n); ++k) dp22[i][j][k] = 0;
  auto setdp22 = letrec([&](auto setdp22, i32 i, i32 j, i32 k, i32 l) -> void {
    if(dp22[i][j][k][l]) return;
    dp22[i][j][k][l] = 1;
    if(i+1 < ngroup) setdp22(i+1,j,k,l);
    if(j > 0) setdp22(i,j-1,k,l);
    if(k > 0) setdp22(i,j,k-1,l);
    if(l+1 < ngroup) setdp22(i,j,k,l+1);
  });
  static bitset<MAXN> dp31[MAXN][MAXN][MAXN];
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(n); ++j) for(i32 k = 0; k < (i32)(n); ++k) dp31[i][j][k] = 0;
  auto setdp31 = letrec([&](auto setdp31, i32 i, i32 j, i32 k, i32 l) -> void {
    if(dp31[i][j][k][l]) return;
    dp31[i][j][k][l] = 1;
    if(i > 0) setdp31(i-1,j,k,l);
    if(j+1 < ngroup) setdp31(i,j+1,k,l);
    if(k+1 < ngroup) setdp31(i,j,k+1,l);
    if(l > 0) setdp31(i,j,k,l-1);
  });
  static bitset<MAXN> dp32[MAXN][MAXN][MAXN];
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(n); ++j) for(i32 k = 0; k < (i32)(n); ++k) dp32[i][j][k] = 0;
  auto setdp32 = letrec([&](auto setdp32, i32 i, i32 j, i32 k, i32 l) -> void {
    if(dp32[i][j][k][l]) return;
    dp32[i][j][k][l] = 1;
    if(i+1 < ngroup) setdp32(i+1,j,k,l);
    if(j > 0) setdp32(i,j-1,k,l);
    if(k > 0) setdp32(i,j,k-1,l);
    if(l+1 < ngroup) setdp32(i,j,k,l+1);
  });
  static bitset<MAXN> dp41[MAXN][MAXN];
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(n); ++j) dp41[i][j] = 0;
  auto setdp41 = letrec([&](auto setdp41, i32 i, i32 j, i32 k) -> void {
    if(dp41[i][j][k]) return;
    dp41[i][j][k] = 1;
    if(i > 0) setdp41(i-1,j,k);
    if(j+1 < ngroup) setdp41(i,j+1,k);
    if(k+1 < ngroup) setdp41(i,j,k+1);
  });
  static bitset<MAXN> dp42[MAXN][MAXN];
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(n); ++j) dp42[i][j] = 0;
  auto setdp42 = letrec([&](auto setdp42, i32 i, i32 j, i32 k) -> void {
    if(dp42[i][j][k]) return;
    dp42[i][j][k] = 1;
    if(i+1 < ngroup) setdp42(i+1,j,k);
    if(j > 0) setdp42(i,j-1,k);
    if(k > 0) setdp42(i,j,k-1);
  });
  static bitset<MAXN> dp51[MAXN][MAXN];
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(n); ++j) dp51[i][j] = 0;
  auto setdp51 = letrec([&](auto setdp51, i32 i, i32 j, i32 k) -> void {
    if(dp51[i][j][k]) return;
    dp51[i][j][k] = 1;
    if(i > 0) setdp51(i-1,j,k);
    if(j+1 < ngroup) setdp51(i,j+1,k);
    if(k+1 < ngroup) setdp51(i,j,k+1);
  });
  static bitset<MAXN> dp52[MAXN][MAXN];
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(n); ++j) dp52[i][j] = 0;
  auto setdp52 = letrec([&](auto setdp52, i32 i, i32 j, i32 k) -> void {
    if(dp52[i][j][k]) return;
    dp52[i][j][k] = 1;
    if(i+1 < ngroup) setdp52(i+1,j,k);
    if(j > 0) setdp52(i,j-1,k);
    if(k > 0) setdp52(i,j,k-1);
  });
  while(num_queries < nq && TM.elapsed() < TL) {
    for(i32 delta = 0; delta < (i32)(nd); ++delta) for(i32 i1 = 0; i1 < (i32)(nd); ++i1) if(i32 i2 = i1+nd-delta; i1 < i2 && i2 < nd) {
      auto s1 = sol[i1], s2 = sol[i2];
      // find X ∈ s1 ∪ s2
      // s.t. s2 <= s2 Δ X <= s1
      for(i32 j = (0); j <= (i32)(ngroup-1); ++j) if((s2&R[j]).any()) {
        for(i32 i = (j-1); i >= (i32)(0); --i) if(j<i+K2 && (s1&R[i]).any()) {
          if(TM.elapsed() > TL) break;
          if(dp1[i][j]) continue;
          auto x = R[i]^R[j];
          if(num_queries < nq) {
            if(query(s2^x, s1) < 0) {
              do { cerr << "main.cpp" ":" "975" "  {" << "\"HERE\", num_queries, i1,i2,i,j" << "} = {"; debug_impl_seq("HERE", num_queries, i1,i2,i,j); cerr << endl << flush; } while(0);
              sol.erase(sol.begin()+i2);
              sol.erase(sol.begin()+i1);
              insert(s2^x);
              insert(s1^x);
              goto lend;
            }else{
              setdp1(i,j);
            }
          }
          break;
        }
      }
    }
    if(nd > 5 && (ngroup / nd <= 6 || nd > 8)) {
      for(i32 delta = 0; delta < (i32)(nd); ++delta) for(i32 i1 = 0; i1 < (i32)(nd); ++i1) if(i32 i2 = i1+nd-delta; i1 < i2 && i2 < nd) {
        auto s1 = sol[i1], s2 = sol[i2];
        for(i32 i = 0; i < (i32)(ngroup-1); ++i) if((s1&R[i]).any()) {
          for(i32 j = (i+1); j <= (i32)(ngroup-1); ++j) if((s2&R[j]).any()) {
            for(i32 k = (j+1); k <= (i32)(ngroup-1); ++k) if((s2&R[k]).any()) {
              if(TM.elapsed() > TL) break;
              auto x = R[i]^R[j]^R[k];
              if(dp41[i][j][k]) continue;
              if(dp42[i][j][k]) continue;
              if(num_queries+1 < nq) {
                if(query(s2^x,s1) < 0) {
                  if(query(s2,s2^x) < 0) {
                    do { cerr << "main.cpp" ":" "1003" "  {" << "\"HERE\", num_queries, i1,i2,i,j,k" << "} = {"; debug_impl_seq("HERE", num_queries, i1,i2,i,j,k); cerr << endl << flush; } while(0);
                    sol.erase(sol.begin()+i2);
                    sol.erase(sol.begin()+i1);
                    insert(s2^x);
                    insert(s1^x);
                    goto lend;
                  }else{
                    setdp42(i,j,k);
                  }
                }else{
                  setdp41(i,j,k);
                }
              }
            }
          }
        }
        //   }
        // FOR(delta, nd) FOR(i1, nd) if(i32 i2 = i1+nd-delta; i1 < i2 && i2 < nd) {
        //   auto s1 = sol[i1], s2 = sol[i2];
        for(i32 i = 0; i < (i32)(ngroup-1); ++i) if((s2&R[i]).any()) {
          for(i32 j = (i+1); j <= (i32)(ngroup-1); ++j) if((s1&R[j]).any()) {
            for(i32 k = (j+1); k <= (i32)(ngroup-1); ++k) if((s1&R[k]).any()) {
              if(TM.elapsed() > TL) break;
              auto x = R[i]^R[j]^R[k];
              if(dp51[i][j][k]) continue;
              if(dp52[i][j][k]) continue;
              if(num_queries+1 < nq) {
                if(query(s2,s2^x) < 0) {
                  if(query(s2^x,s1) < 0) {
                    do { cerr << "main.cpp" ":" "1033" "  {" << "\"HERE\", num_queries, i1,i2,i,j,k" << "} = {"; debug_impl_seq("HERE", num_queries, i1,i2,i,j,k); cerr << endl << flush; } while(0);
                    sol.erase(sol.begin()+i2);
                    sol.erase(sol.begin()+i1);
                    insert(s2^x);
                    insert(s1^x);
                    goto lend;
                  }else{
                    setdp52(i,j,k);
                  }
                }else{
                  setdp51(i,j,k);
                }
              }
            }
          }
        }
      }
      for(i32 delta = 0; delta < (i32)(nd); ++delta) for(i32 i1 = 0; i1 < (i32)(nd); ++i1) if(i32 i2 = i1+nd-delta; i1 < i2 && i2 < nd) {
        auto s1 = sol[i1], s2 = sol[i2];
        for(i32 i = 0; i < (i32)(ngroup-1); ++i) if((s1&R[i]).any()) {
          for(i32 j = (i+1); j <= (i32)(ngroup-1); ++j) if(j < i+K2 && (s2&R[j]).any()) {
            for(i32 k = (j+1); k <= (i32)(ngroup-1); ++k) if((s2&R[k]).any()) {
              for(i32 l = (k+1); l <= (i32)(ngroup-1); ++l) if(l < k+K2 && (s1&R[l]).any()) {
                if(TM.elapsed() > TL) break;
                auto x = R[i]^R[j]^R[k]^R[l];
                if(dp21[i][j][k][l]) continue;
                if(dp22[i][j][k][l]) continue;
                if(num_queries+1 < nq) {
                  if(query(s2^x,s1) < 0) {
                    if(query(s2,s2^x) < 0) {
                      do { cerr << "main.cpp" ":" "1064" "  {" << "\"HERE\", num_queries, i1,i2,i,j,k,l" << "} = {"; debug_impl_seq("HERE", num_queries, i1,i2,i,j,k,l); cerr << endl << flush; } while(0);
                      sol.erase(sol.begin()+i2);
                      sol.erase(sol.begin()+i1);
                      insert(s2^x);
                      insert(s1^x);
                      goto lend;
                    }else{
                      setdp22(i,j,k,l);
                    }
                  }else{
                    setdp21(i,j,k,l);
                  }
                }
              }
            }
          }
        }
        // }
        // FOR(delta, nd) FOR(i1, nd) if(i32 i2 = i1+nd-delta; i1 < i2 && i2 < nd) {
        //   auto s1 = sol[i1], s2 = sol[i2];
        for(i32 i = 0; i < (i32)(ngroup-1); ++i) if((s2&R[i]).any()) {
          for(i32 j = (i+1); j <= (i32)(ngroup-1); ++j) if(j < i+K2 && (s1&R[j]).any()) {
            for(i32 k = (j+1); k <= (i32)(ngroup-1); ++k) if((s1&R[k]).any()) {
              for(i32 l = (k+1); l <= (i32)(ngroup-1); ++l) if(l < k+K2 && (s2&R[l]).any()) {
                if(TM.elapsed() > TL) break;
                auto x = R[i]^R[j]^R[k]^R[l];
                if(dp31[i][j][k][l]) continue;
                if(dp32[i][j][k][l]) continue;
                if(num_queries+1 < nq) {
                  if(query(s2^x,s1) < 0) {
                    if(query(s2,s2^x) < 0) {
                      do { cerr << "main.cpp" ":" "1096" "  {" << "\"HERE\", num_queries, i1,i2,i,j,k,l" << "} = {"; debug_impl_seq("HERE", num_queries, i1,i2,i,j,k,l); cerr << endl << flush; } while(0);
                      sol.erase(sol.begin()+i2);
                      sol.erase(sol.begin()+i1);
                      insert(s2^x);
                      insert(s1^x);
                      goto lend;
                    }else{
                      setdp31(i,j,k,l);
                    }
                  }else{
                    setdp32(i,j,k,l);
                  }
                }
              }
            }
          }
        }
      }
      if(num_queries+1 < nq) {
        for(i32 delta = 0; delta < (i32)(nd); ++delta) for(i32 i1 = 0; i1 < (i32)(nd); ++i1) if(i32 i2 = i1+nd-delta; i1 < i2 && i2 < nd) {
          if(TM.elapsed() > TL) break;
          auto s1 = sol[i1], s2 = sol[i2];
          vector<S> X;
          for(i32 i = 0; i < (i32)(ngroup); ++i) if(((s1|s2)&R[i]).any()) {
            X.emplace_back(R[i]);
          }
          if(X.size() <= 13) {
            auto a = solve_ss(X, {s2,s1});
            if(a[0] != s2 && a[0] != s1) {
              sol.erase(sol.begin()+i2);
              sol.erase(sol.begin()+i1);
              insert(a[0]);
              insert(a[1]);
              goto lend;
            }
          }
        }
      }
    }
    if(num_queries+1 < nq) {
      for(i32 delta = 0; delta < (i32)(nd); ++delta) for(i32 i1 = 0; i1 < (i32)(nd); ++i1) if(i32 i2 = i1+nd-delta; i1 < i2 && i2 < nd) {
        if(TM.elapsed() > TL) break;
        auto s1 = sol[i1], s2 = sol[i2];
        i32 count1 = 0;
        for(i32 i = 0; i < (i32)(ngroup); ++i) if((s1&R[i]).any()) {
          count1 += 1;
        }
        if(count1 == 1) continue;
        vector<S> elems;
        for(i32 i = 0; i < (i32)(ngroup); ++i) if(((s1|s2)&R[i]).any()) {
          elems.emplace_back(R[i]);
        }
        if(elems.size() <= 5) {
          auto a = solve_ss(elems, {s2,s1});
          if(a[0] != s2 && a[0] != s1) {
            sol.erase(sol.begin()+i2);
            sol.erase(sol.begin()+i1);
            insert(a[0]);
            insert(a[1]);
            goto lend;
          }
        }else{
          reverse(begin(elems), end(elems));
          auto best = run_ckk(elems, {s2,s1}, true);
          if(best[0] != s2) {
            sol.erase(sol.begin()+i2);
            sol.erase(sol.begin()+i1);
            insert(best[0]);
            insert(best[1]);
            goto lend;
          }
        }
      }
    }
    break;
  lend:;
  }
  while(num_queries < nq && TM.elapsed() < TL) {
    for(i32 delta = 0; delta < (i32)(nd); ++delta) for(i32 i1 = 0; i1 < (i32)(nd); ++i1) if(i32 i2 = i1+nd-delta; i1 < i2 && i2 < nd) {
      if(TM.elapsed() > TL) break;
      auto s1 = sol[i1], s2 = sol[i2];
      vector<S> X;
      for(i32 i = 0; i < (i32)(n); ++i) if(s1[i]||s2[i]) {
        X.emplace_back(singleton(i));
      }
      if(X.size() <= 13) {
        auto a = solve_ss(X, {s2,s1});
        if(a[0] != s2 && a[0] != s1) {
          sol.erase(sol.begin()+i2);
          sol.erase(sol.begin()+i1);
          insert(a[0]);
          insert(a[1]);
          goto lend2;
        }
      }
    }
    break;
  lend2:;
  }
  do { cerr << "main.cpp" ":" "1204" "  {" << "\"after opt\", num_queries, nq" << "} = {"; debug_impl_seq("after opt", num_queries, nq); cerr << endl << flush; } while(0);
  while(num_queries+2 <= nq && TM.elapsed() < 1.97) {
    auto i1 = 0, i2 = nd-1;
    auto s1 = sol[i1], s2 = sol[i2];
    S x;
    double p = (4.0 / (s1.count()+s2.count()));
    for(i32 i = 0; i < (i32)(n); ++i) if((s1[i] || s2[i])&&rng.randomDouble() < p) x[i]=1;
    auto s2x = s2^x;
    if(query(s2, s2x) < 0 && query(s2x, s1) < 0) {
      sol.erase(sol.begin());
      sol.pop_back();
      insert(s2x);
      insert(s1^x);
    }
  }
  do { cerr << "main.cpp" ":" "1223" "  {" << "num_queries,nq" << "} = {"; debug_impl_seq(num_queries,nq); cerr << endl << flush; } while(0);
  while(num_queries < nq) {
    num_queries += 1;
    cout << "1 1 0 1" << endl;
    char c; cin>>c;
  }
  vector<i32> col(n);
  for(i32 i = 0; i < (i32)(n); ++i) col[i] = rng.random32(nd);
  for(i32 i = 0; i < (i32)(nd); ++i) for(i32 j = 0; j < (i32)(n); ++j) if(sol[i][j]) col[j] = i;
  for(i32 i = 0; i < (i32)(n); ++i) cout << col[i] << ' ';
  cout << endl;
  do { cerr << "main.cpp" ":" "1237" "  {" << "insert_pos" << "} = {"; debug_impl_seq(insert_pos); cerr << endl << flush; } while(0);
  do { cerr << "main.cpp" ":" "1238" "  {" << "cost_insert" << "} = {"; debug_impl_seq(cost_insert); cerr << endl << flush; } while(0);
// #ifdef LOCAL
// #ifndef CP_GEN
//   { ofstream os("insert_positions", std::ios::app);
//     os << nd;
//     FOR(i,nd) os << " " << insert_pos[i];
//     os << endl << flush;
//     os.close();
//   }
// #endif
// #endif
  cerr << "[DATA] time = " << TM.elapsed() << endl;
}
i32 main(){
  TM.reset();
  rng.reset(time(0));
  read();
  A.resize(n);
  solve();
  cerr << "Elapsed: " << TM.elapsed() << endl;
  cerr << "Hits: " << hits << endl;
  cerr << "DFS Hits: " << dfs_hits << endl;
  cerr << "LP Hits: " << lp_hits << endl;
  cerr << "Tot queries: " << tot_queries << endl;
  return 0;
}
double get_elapsed() {
  return TM.elapsed();
}
