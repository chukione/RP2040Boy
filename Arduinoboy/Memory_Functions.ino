
boolean checkMemory()
{
  byte chk;
  #if !defined(USE_DUE) && !defined(USE_PICO)
  for(int m=0;m<4;m++){
    chk =  EEPROM.read(MEM_CHECK+m);
    if(chk != defaultMemoryMap[MEM_CHECK+m]) {
      return false;
    }
  }
  #endif
  return true;
}

void initMemory(boolean reinit)
{
  if(!alwaysUseDefaultSettings) {
    #if !defined(USE_DUE) && !defined(USE_PICO)
    if(reinit || !checkMemory()) {
      for(int m=0;m<MEM_MAX;m++){
        EEPROM.write(m,defaultMemoryMap[m]);
      }
    }
    #endif
    loadMemory();
  } else {
    for(int m=0;m<MEM_MAX;m++){
      memory[m] = defaultMemoryMap[m];
    }
  }
  changeTasks();
}


void loadMemory()
{
  #if !defined(USE_DUE) && !defined(USE_PICO)
  for(int m=0; m<MEM_MAX;m++){
     memory[m] = EEPROM.read(m);
  }
  #endif
  changeTasks();
}

void printMemory()
{
  for(int m=0;m<MEM_MAX;m++){
    serial->println(memory[m],HEX);
  }
}

void saveMemory()
{
  #if !defined(USE_DUE) && !defined(USE_PICO)
  for(int m=0; m<MEM_MAX;m++){
    EEPROM.write(m,memory[m]);
  }
  changeTasks();
  #endif
}

void changeTasks()
{
  midioutByteDelay = memory[MEM_MIDIOUT_BYTE_DELAY] * memory[MEM_MIDIOUT_BYTE_DELAY+1];
  midioutBitDelay = memory[MEM_MIDIOUT_BIT_DELAY] * memory[MEM_MIDIOUT_BIT_DELAY+1];
}
