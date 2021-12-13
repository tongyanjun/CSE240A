//========================================================//
//  predictor.h                                           //
//  Header file for the Branch Predictor                  //
//                                                        //
//  Includes function prototypes and global predictor     //
//  variables and defines                                 //
//========================================================//

#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <stdint.h>
#include <stdlib.h>

//
// Student Information
//
extern const char *studentName;
extern const char *studentID;
extern const char *email;

//------------------------------------//
//      Global Predictor Defines      //
//------------------------------------//
#define NOTTAKEN  0
#define TAKEN     1

// The Different Predictor Types
#define STATIC      0
#define GSHARE      1
#define TOURNAMENT  2
#define CUSTOM      3
extern const char *bpName[];

// Definitions for 2-bit counters
#define SN  0			// predict NT, strong not taken
#define WN  1			// predict NT, weak not taken
#define WT  2			// predict T, weak taken
#define ST  3			// predict T, strong taken

// Definitions for 3-bit counters
#define SSN  0			// predict NT, strong strong not taken
#define MSN  1			// predict NT, medium strong not taken
#define MWN  2          // predict NT, medium weak not taken
#define WWN  3          // predict NT, weak weak not taken
#define WWT  4			// predict T, weak weak taken
#define MWT  5          // predict T, medium weak taken
#define MST	 6          // predict T, medium strong taken
#define SST  7          // predict T, strong strong taken

// Definitions for choice counters
#define SL  0           // strong choose global predictor
#define WL  1           // weak choose global predictor
#define WG  2           // weak choose local predictor
#define SG  3           // strong choose local predictor

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//
extern int ghistoryBits; // Number of bits used for Global History
extern int lhistoryBits; // Number of bits used for Local History
extern int pcIndexBits;  // Number of bits used for PC index
extern int bpType;       // Branch Prediction Type
extern int verbose;

//------------------------------------//
//    Predictor Function Prototypes   //
//------------------------------------//

// Initialize the predictor
//
void init_predictor();
void init_gshare();
void init_tournament(uint32_t local_wn);
void init_custom();

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_prediction(uint32_t pc);
uint8_t gs_predict(uint32_t pc);
uint8_t tournament_predict(uint32_t pc);
uint8_t custom_predict(uint32_t pc);
uint8_t global_local_predict(uint32_t pc, uint32_t global_index, uint32_t local_wt);

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_predictor(uint32_t pc, uint8_t outcome);
void train_gs(uint32_t pc, uint8_t outcome);
void train_tournament(uint32_t pc, uint8_t outcome);
void train_custom(uint32_t pc, uint8_t outcome);
void train_global_local(uint32_t pc, uint8_t outcome, uint32_t global_index, uint32_t local_st, uint32_t local_sn, uint32_t local_wt);

// Cleanup
void free_predictor();

#endif
