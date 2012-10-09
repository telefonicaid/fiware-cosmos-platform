#ifndef _H_KV_INPUT_VECTOR
#define _H_KV_INPUT_VECTOR

#include <vector>

#include "samson/common/coding.h"                               // KV*
#include "samson/module/Operation.h"    //  OperationInputCompareFunction
namespace samson {
/*
 * Class to process alll input key-values for a map, reduce or parseOut operation
 * Vector key-values to sort and process all input channels
 */

class KVInputVectorBase {
  public:

    KV *kv;   // Dynamic Vector of KV elements
    KV **_kv;   // Dynamic Vector with pointers to kv

    size_t max_num_kvs;   // Allocation size
    size_t num_kvs;   // Real number of kvs in the vectors

    // Define the maximum number of key-values we will add
    void prepareInput(size_t _max_num_kvs);
};

class KVInputVector : public KVInputVectorBase {
  public:
    // Constructors & destructors
    explicit KVInputVector(Operation *operation);
    explicit KVInputVector(int _num_inputs);   // Old constructor ( only used in batch tech, to be removed in samson 0.7 )
    ~KVInputVector();

    // Function to add kv
    void addKVs(int input, KVInfo info, KV *kvs);

    // Function to add key-values to the input vector
    void addKVs(int input, KVInfo info, char *data);

    // global sort function key - input - value used in reduce operations
    void sort();

    // Special sort where first part of the added key-value is not sorted and the other half is already sorted
    void sortAndMerge(size_t middle_pos);

    // Init and getNext functions allows to retrieve key-values in groups with the same key
    void Init();
    KVSetStruct *GetNext();

  private:
    DataInstance *keyDataInstance_;   // Data instance for the key ( common to all inputs )
    std::vector<DataInstance *> valueDataInstances_;   // Data instances for the values

    int num_inputs_;   // Number of input channels ( 1 in maps and parseOut , N in reduce operations )

    KVSetStruct *inputStructs_;   // Structure used to process key-values contained in this input vector in reduce operations

    size_t pos_begin_, pos_end_;   // State variables used across calls to init and getNext
};
}

#endif  // ifndef _H_KV_INPUT_VECTOR
