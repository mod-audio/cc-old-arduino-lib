#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "config.h"
#include <stdint.h>
#include "mode.h"
#include "assignment.h"

#ifndef MAX_ASSIGNMENTS
#define MAX_ASSIGNMENTS 2
#endif

#ifndef VALUE_CHANGE_TOLERANCE
#define VALUE_CHANGE_TOLERANCE 0.01
#endif

// class Update;

/*
************************************************************************************************************************
This class models a physical generic actuator.
************************************************************************************************************************
*/
class Actuator{
public:
    const char*         name;           // name displayed to user on mod-ui.
    uint8_t             name_length;        // name size.
    uint8_t             id;
    Mode**              modes;
    uint16_t*           steps;

    uint8_t             num_assignments;    //how many parameters the actuator can support simultaneously.
    uint8_t             num_modes;          //how many modes the actuator have.
    uint8_t             num_steps;  //size of steps list.

    uint8_t             assignments_occupied;  //how many assignment slots the actuator have occupied until now.

    bool                changed;

    float               old_value;
    float               value;

    Assignment*         current_assig;
    Assignment*         assig_list_head;

    Actuator(const char* name, uint8_t id, uint8_t num_assignments, Mode** modes, uint8_t num_modes, uint16_t* steps, uint8_t num_steps);

    ~Actuator();

    void init();

    Assignment* getListHead();

    Assignment* getListTail();


    void printList();//vv

    // associates a pointer to the assignment list contained in actuators class.
    bool assign(const uint8_t* ctrl_data);/// Deve verificar se o numero de endereçamentos ultrapassa o numero de slots;

    // frees a parameter slot.
    bool unassign(uint8_t assignment_id);

    bool supportMode(uint8_t relevant_properties, uint8_t property_values);

    // checks if the value in the actuator changed.
    bool checkChange();

    // this function runs after the message is sent. It serves to clear the changed flag, which indicates that the actuator
    // has changed its value.
    void postMessageRotine();

    // returns actuators descriptor size
    uint16_t descriptorSize();

    // writes the actuator descriptor on the buffer.
    int getDescriptor(uint8_t* buffer);

    // writes the current assignment id and value on buffer (used in data request).
    int getUpdate(uint8_t *buffer);

    // change current_assignment to next assignment.
    void nextAssignment();

    // change current_assignment to previous assignment.
    void previousAssignment();

    // These functions are supposed to be implemented in a subclass.
    ///////////////////////////////////////////////////////////

    // process value read in getValue using the assingment mode as reference.
    virtual void calculateValue()=0;

    // reads analog or digital value
    virtual float getValue()=0;

    // This function will run after message has been sent.
    virtual void postMessageChanges()=0;

    // This function will run after an assignent has been made.
    virtual void assignmentRotine()=0;

    /////////////////////////////////////////////////////////////

};

#endif
