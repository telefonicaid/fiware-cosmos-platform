/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
 */

#ifndef _H_SAMSON_fiware_pos_Users
#define _H_SAMSON_fiware_pos_Users

#include <logMsg/logMsg.h>

#include <samson/modules/fiware_pos/Users_base.h>

namespace samson {
namespace fiware_pos {

class Users : public Users_base {
  public:
    void Init() {
      // Initialize to a large initial number of users to avoid reallocating several times

      // Preallocate a number of users...
#define INITIAL_NUMBER_OF_USERS 10000
      user_ticksSetLength(INITIAL_NUMBER_OF_USERS);
      // ... but start with 0
      user_ticksSetLength(0);
      num_users.value = 0;
    }

    void AddUser(const samson::fiware_pos::UserTick& user_tick)
    {
        samson::fiware_pos::UserTick *new_user = user_ticksAdd();
        new_user->user_id = user_tick.user_id;
        new_user->incoming = user_tick.incoming;
        new_user->timestamp = user_tick.timestamp;
        ++(num_users.value);
    }

    void RemoveUser(int user_index)
    {
      if (user_ticksErase(user_index) == false) {
        LM_E(("Error erasing user at index:%d from list.size:%lu", user_index, user_ticks_length));
        return;
      }
      --(num_users.value);
    }
};

} // end of namespace samson
} // end of namespace fiware_pos

#endif
