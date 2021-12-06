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

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

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
    init_tournament();
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
init_tournament() {

}

void
init_custom() {

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
  uint8_t predict = global_bht[index];

  if (predict < WT) return NOTTAKEN;
  return TAKEN;
}

uint8_t
tournament_predict(uint32_t pc) {
  return NOTTAKEN;
}

uint8_t
custom_predict(uint32_t pc) {
  return NOTTAKEN;
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
  global_bhr = (global_bhr << 1) | outcome;
}

void
train_tournament(uint32_t pc, uint8_t outcome) {
  
}

void
train_custom(uint32_t pc, uint8_t outcome) {
  
}