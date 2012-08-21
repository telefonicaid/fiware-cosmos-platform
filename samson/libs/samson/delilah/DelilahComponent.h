
#ifndef _H_DELILAH_COMPONENT
#define _H_DELILAH_COMPONENT

#include "au/CronometerSystem.h"
#include "au/ErrorManager.h"        // au::ErrorManager

#include "samson/network/Message.h"  // Message::MessageCode
#include "samson/network/Packet.h"  // samson::Packet
#include <cstring>


namespace samson {
class Delilah;

class DelilahComponent {
  friend class Delilah;
  bool component_finished;     // Flag to be removed when indicated by user

public:

  au::CronometerSystem cronometer;

  typedef enum {
    push,                      // DataSource based push operation
    pop,
    worker_command,            // Command send to each worker
  }DelilaComponentType;

  DelilaComponentType type;

  size_t id;                   // Identifier of this component
  Delilah *delilah;            // Pointer to delilah to notify finish or show messages
  std::string concept;         // Concept of this component to print list of components

  double progress;             // Information about progress of this task
  bool hidden;                 // Flag to not show information on screen about this

  std::ostringstream output;   // Output to be shown on screen
  bool print_output_at_finish;      // Flag to determine if we have to show result at the end

  au::ErrorManager error;      // Manager of the error in this operation

  DelilahComponent(DelilaComponentType _type);
  virtual ~DelilahComponent() {
  };                           // Virtual destructor necessary in this class since subclasses are deleted using parent pointers

  void setId(Delilah *_delilah,  size_t _id);
  virtual void receive(const PacketPointer& packet) = 0;
  virtual void review() {
  };

  // General function to give a long description status ( used when typing ps X )
  virtual std::string getStatus() = 0;
  virtual std::string getExtraStatus() {
    return "";
  }

  // Check if the component is finished
  bool isComponentFinished();

  // A string description of the type of operation
  std::string getTypeName();

  // One line description to show in lists
  std::string getStatusDescription();
  std::string getIdAndConcept();

  size_t getId() {
    return id;
  }

  std::string getConcept() {
    return concept;
  }

  void set_print_output_at_finish() {
    print_output_at_finish = true;
  }

protected:

  void setConcept(std::string _concept);
  void setProgress(double p);

  // Command to mark this delilah component finished
  void setComponentFinished();
  void setComponentFinishedWithError(std::string error_message);
};
}

#endif  // ifndef _H_DELILAH_COMPONENT
