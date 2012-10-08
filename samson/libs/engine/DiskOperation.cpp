#include "engine/DiskManager.h"         // engine::DiskManager
#include "engine/DiskOperation.h"       // Own interface
#include "engine/Engine.h"              // engine::Engine
#include "engine/ReadFile.h"            // engine::ReadFile
#include "logMsg/logMsg.h"              // LM_T
#include "logMsg/traceLevels.h"         // LmtDisk

namespace engine {
DiskOperation::DiskOperation() {
  // Some default values
  read_buffer = NULL;
}

DiskOperation::~DiskOperation() {
}

DiskOperation *DiskOperation::newReadOperation(char *data, std::string fileName, size_t offset, size_t size,
                                               size_t _listenerId) {
  DiskOperation *o = new DiskOperation();

  o->fileName = fileName;
  o->type = DiskOperation::read;
  o->read_buffer = data;
  o->size = size;
  o->offset = offset;
  o->addListener(_listenerId);

  o->environment.Set("type", "read");

  return o;
}

DiskOperation *DiskOperation::newReadOperation(std::string fileName, size_t offset, size_t size,
                                               SimpleBuffer simpleBuffer,
                                               size_t _listenerId) {
  DiskOperation *o = new DiskOperation();

  o->fileName = fileName;
  o->type = DiskOperation::read;
  o->read_buffer = simpleBuffer.data();
  o->size = size;
  o->offset = offset;
  o->addListener(_listenerId);

  o->environment.Set("type", "read");

  return o;
}

DiskOperation *DiskOperation::newWriteOperation(BufferPointer buffer,  std::string fileName, size_t _listenerId) {
  DiskOperation *o = new DiskOperation();

  o->fileName = fileName;
  o->type = DiskOperation::write;
  o->buffer = buffer;
  o->size = buffer->size();
  o->offset = 0;
  o->addListener(_listenerId);

  o->environment.Set("type", "write");

  return o;
}

DiskOperation *DiskOperation::newAppendOperation(BufferPointer buffer,  std::string fileName, size_t _listenerId) {
  DiskOperation *o = new DiskOperation();

  o->fileName = fileName;
  o->type = DiskOperation::append;
  o->buffer = buffer;
  o->size = buffer->size();
  o->offset = 0;
  o->addListener(_listenerId);

  o->environment.Set("type", "append");

  return o;
}

DiskOperation *DiskOperation::newRemoveOperation(std::string fileName, size_t _listenerId) {
  DiskOperation *o = new DiskOperation();

  o->fileName = fileName;
  o->type = DiskOperation::remove;
  o->size = 0;
  o->addListener(_listenerId);

  o->environment.Set("type", "remove");

  return o;
}

std::string DiskOperation::getDescription() {
  std::ostringstream o;

  switch (type) {
    case write:
      o << "Write to file: '" << fileName << "' Size:" << au::str(size, "B");
      break;
    case append:
      o << "Append to file: '" << fileName << "' Size:" << au::str(size, "B");
      break;
    case read:
      o << "Read from file: '" << fileName << "' Size:" << au::str(size, "B") << " [" << size << "B] Offset:" << offset;
      break;
    case remove:
      o << "Remove file: '" << fileName << "'";
      break;
  }


  return o.str();
}

std::string DiskOperation::getShortDescription() {
  std::ostringstream o;

  switch (type) {
    case write:
      o << "W:" << au::str(size);
      break;
    case append:
      o << "A:" << au::str(size);
      break;
    case read:
      o << "R:" << au::str(size);
      break;
    case remove:
      o << "X";
      break;
  }


  return o.str();
}

void DiskOperation::setError(std::string message) {
  std::ostringstream o;

  o << message << " ( " << getDescription() << " )";
  error.set(o.str());
}

void DiskOperation::run() {
  // Detect some slow disk access if rate is going bellow 10Mb/s in large operations
  // double alarm_time_secs = std::max(  (double) size / 10000000.0 , 5.0 );
  // au::ExecesiveTimeAlarm alarm( au::str("Disk Operation '%s;",getDescription().c_str() , alarm_time_secs ) );

  LM_T(LmtDisk,  ("START DiskManager: Running operation %s", getDescription().c_str()));

  if (type == DiskOperation::write) {
    // Create a new file


    LM_T(LmtDisk, ("DiskManager: Opening file %s to write", fileName.c_str()));
    FILE *file = fopen(fileName.c_str(), "w");
    if (!file) {
      LM_E(("Error opening file for writing, fileName:%s, errno:%d", fileName.c_str(), errno));
      setError("Error opening file");
    } else {
      if (size > 0) {
        if (fwrite(buffer->data(), size, 1, file) == 1) {
          fflush(file);
        } else {
          LM_E(("Error writing data to file, fileName:%s, errno:%d", fileName.c_str(), errno));
          setError("Error writing data to the file");
        }
      }
      LM_T(LmtDisk, ("DiskManager: write operation on file %s completed", fileName.c_str()));
      fclose(file);
    }
  }

  if (type == DiskOperation::append) {
    // Create a new file


    LM_T(LmtDisk, ("DiskManager: Opening file %s to append", fileName.c_str()));
    FILE *file = fopen(fileName.c_str(), "a");
    if (!file) {
      setError("Error opening file");
    } else {
      if (size > 0) {
        if (buffer == NULL) {
          LM_X(1, ("Internal error"));
        }
        if (fwrite(buffer->data(), size, 1, file) == 1) {
          fflush(file);
        } else {
          setError("Error writing data to the file");
        }
      }

      fclose(file);
    }
  }


  if (type == DiskOperation::read) {
    LM_T(LmtDisk, ("DiskManager: Opening file %s to read", fileName.c_str()));

    // Get the Read file from the Manager
    ReadFile *rf = diskManager->fileManager_.GetReadFile(fileName);

    if (!rf->IsValid()) {
      LM_E(("Internal error: Not valid read file %s", fileName.c_str()));
      setError("Internal error: Not valid read file");
    } else {
      if (rf->Seek(offset)) {
        LM_E(("Error while seeking data from file %s", fileName.c_str()));
        setError(au::str("Error while seeking data from file %s", fileName.c_str()));
      }


      if (rf->Read(read_buffer, size)) {
        LM_E(("Error while reading data from file %s", fileName.c_str()));
        setError(au::str("Error while reading data from file %s", fileName.c_str()));
      }
    }
  }

  if (type == DiskOperation::remove) {
    LM_T(LmtDisk, ("DiskManager: Removing file %s", fileName.c_str()));

    // Remove the file
    int c = ::remove(fileName.c_str());
    if (c != 0) {
      setError("Error while removing file");
    }
  }

  LM_T(LmtDisk, ("FINISH DiskManager: Finished with file %s, ready to finishDiskOperation", fileName.c_str()));
  // Notify to the engine
}

void DiskOperation::getInfo(std::ostringstream& output) {
  output << "<disk_operation>\n";

  output << "<type>";
  switch (type) {
    case read:
      output << "read";
      break;
    case write:
      output << "write";
      break;
    case append:
      output << "append";
      break;
    case remove:
      output << "remove";
      break;

    default:
      break;
  }
  output << "</type>\n";

  output << "<file_name>" << fileName << "</file_name>\n";

  output << "<size>" << size << "</size>\n";
  output << "<offset>" << offset << "</offset>\n";

  output << "</disk_operation>\n";
}
}
