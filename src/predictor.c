//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Yanjun Tong";
const char *studentID   = "A59004168";
const char *email       = "yj.sjtu@gmail.com";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
uint32_t pc_mask;

uint32_t *global_bht;
uint32_t global_bhr;
uint32_t global_mask;

uint32_t *choice_bht;
uint32_t *local_bhrs;
uint32_t *local_bht;
uint32_t local_mask;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//
// Free the predictor
void 
free_predictor() {
  switch (bpType) {
    case GSHARE:
      free(global_bht);
      break;
    case TOURNAMENT:
      free(global_bht);
      free(choice_bht);
      free(local_bhrs);
      free(local_bht);
      break;
    case CUSTOM:
      free(global_bht);
      free(choice_bht);
      free(local_bhrs);
      free(local_bht);
      break;
    default:
      break;
  }
}

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  // initialize pc mask
  for(int i=0; i<pcIndexBits; i++) pc_mask |= (1 << i);

  switch (bpType)
  {
  case GSHARE:
    init_gshare();
    break;
  case TOURNAMENT:
    init_tournament(WN);
    break;
  case CUSTOM:
    init_custom();
    break;
  
  default:
    break;
  }
}

void
init_gshare() {
  int size = 1 << ghistoryBits;
  global_bht = (uint32_t*) malloc(sizeof(uint32_t)*size);
  // global history initialized with NOTTAKEN
  global_bhr = 0;
  for(int i=0; i<ghistoryBits; i++) global_mask |= (1 << i);
  // initial bht with weakly not taken
  for(int i=0; i<size; i++) {
    global_bht[i] = WN;
  }
}

void
init_tournament(uint32_t local_wn) {
  // global history initialized with NOTTAKEN
  // printf("enter init");
  global_bhr = 0;
  for(int i=0; i<ghistoryBits; i++) global_mask |= (1 << i);
  // global_bht
  int size = 1 << ghistoryBits;
  global_bht = (uint32_t*) malloc(sizeof(uint32_t)*size);
  for(int i=0; i<size; i++) {
    global_bht[i] = WN;
  }
  // choice_bht, initialized to Weakly select the Global Predictor
  choice_bht = (uint32_t*) malloc(sizeof(uint32_t)*size);
  for(int i=0; i<size; i++) {
    choice_bht[i] = WG;
  }
  // local bhrs
  size = 1 << pcIndexBits;
  local_bhrs = (uint32_t*) malloc(sizeof(uint32_t)*size);
  for(int i=0; i<size; i++) {
    local_bhrs[i] = 0;
  }
  for(int i=0; i<lhistoryBits; i++) local_mask |= (1 << i);
  // local bht
  size = 1 << lhistoryBits;
  local_bht = (uint32_t*) malloc(sizeof(uint32_t)*size);
  for(int i=0; i<size; i++) {
    local_bht[i] = local_wn;
  }
}

void
init_custom() {
  // give initial value for args
  ghistoryBits = 13;
  lhistoryBits = 11;
  pcIndexBits = 11;
  
  // the rest inital process will follow the same pattern as tournament
  init_tournament(WWN);
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      return gs_predict(pc);
    case TOURNAMENT:
      return tournament_predict(pc);
    case CUSTOM:
      return custom_predict(pc);
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

uint8_t
gs_predict(uint32_t pc) {
  // XOR the global history register with the lower bits (same length as the global history) of pc
  uint32_t pcbits = pc & global_mask;
  uint32_t bhr = global_bhr & global_mask;
  uint32_t index = pcbits ^ bhr;
  uint32_t predict = global_bht[index];

  if (predict < WT) return NOTTAKEN;
  return TAKEN;
}

uint8_t
tournament_predict(uint32_t pc) {
  return global_local_predict(pc, global_bhr & global_mask, WT);
}

uint8_t
custom_predict(uint32_t pc) {
  // cooperate with GSHARE XOR
  uint32_t index = (pc & global_mask) ^ (global_bhr & global_mask);
  return global_local_predict(pc, index, WWT);
}

uint8_t
global_local_predict(uint32_t pc, uint32_t global_index, uint32_t local_wt) {
  if(choice_bht[global_index] < WG) {
    uint32_t pcbits = pc & pc_mask;
    uint32_t local_bhr = local_bhrs[pcbits] & local_mask;
    if(local_bht[local_bhr] < local_wt) return NOTTAKEN;
    return TAKEN;
  } else {
    if(global_bht[global_index] < WT) return NOTTAKEN;
    return TAKEN;
  }
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  switch (bpType) {
    case GSHARE:
      train_gs(pc, outcome);
      break;
    case TOURNAMENT:
      train_tournament(pc, outcome);
      break;
    case CUSTOM:
      train_custom(pc, outcome);
      break;
    default:
      break;
  }
}

void
train_gs(uint32_t pc, uint8_t outcome) {
  uint32_t pcbits = pc & global_mask;
  uint32_t bhr = global_bhr & global_mask;
  uint32_t index = pcbits ^ bhr;
  if(outcome == TAKEN) {
    if(global_bht[index] < ST) global_bht[index]++;
  } else {
    if(global_bht[index] > SN) global_bht[index]--;
  }
  // update global branch history register
  global_bhr = ((global_bhr << 1) | outcome) & global_mask;
}

void
train_tournament(uint32_t pc, uint8_t outcome) {
  train_global_local(pc, outcome, global_bhr & global_mask, ST, SN);
}

void
train_custom(uint32_t pc, uint8_t outcome) {
  uint32_t index = (pc & global_mask) ^ (global_bhr & global_mask);
  train_global_local(pc, outcome, index, SST, SSN);
}

void
train_global_local(uint32_t pc, uint8_t outcome, uint32_t global_index, uint32_t local_st, uint32_t local_sn) {
  uint32_t pcbits = pc & pc_mask;
  uint32_t local_bhr = local_bhrs[pcbits] & local_mask;
  uint32_t local_predict = local_bht[local_bhr]<WT?NOTTAKEN:TAKEN;
  // update local bht
  if(outcome == TAKEN) {
    if(local_bht[local_bhr] < local_st) local_bht[local_bhr]++;
  } else {
    if(local_bht[local_bhr] > local_sn) local_bht[local_bhr]--;
  }
  
  uint32_t global_predict = global_bht[global_index]<WT?NOTTAKEN:TAKEN;
  // update global bht
  if(outcome == TAKEN) {
    if(global_bht[global_index] < ST) global_bht[global_index]++;
  } else {
    if(global_bht[global_index] > SN) global_bht[global_index]--;
  }

  // update choice bht
  if(local_predict == outcome && global_predict != outcome && choice_bht[global_index] > SL) {
    choice_bht[global_index]--;
  } else if(local_predict != outcome && global_predict == outcome && choice_bht[global_index] < SG) {
    choice_bht[global_index]++;
  }

  // update local bhr
  local_bhrs[pcbits] = ((local_bhrs[pcbits] << 1) | outcome) & local_mask;
  // update global bhr
  global_bhr = ((global_bhr << 1) | outcome) & global_mask;
}