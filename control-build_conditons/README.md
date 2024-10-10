# Building Conditions

## Installations
To get started, install the necessary dependencies by running the following commands:

```bash
sudo apt-get update
sudo apt-get install qtbase5-dev qtdeclarative5-dev qttools5-dev qt5-qmake qttools5-dev-tools libqt5printsupport5
sudo apt-get install libbson-dev
sudo apt update
sudo apt install pkg-config


## Usage

### User Guide for Building Conditions and Actions

#### Overview

The system provides a flexible interface for creating both simple and complex conditions, allowing you to design intricate logical flows based on sensor data. You can combine multiple conditions using logical operators such as AND and OR, enabling you to create sophisticated logic with multiple layers of conditions.

The GUI ensures that only valid combinations of conditions and actions are allowed. Buttons and options that lead to invalid or incomplete conditions will remain disabled until all required fields are correctly filled in.

### Step 1: Building Conditions

1. **Start Condition Creation:** The first window presents an `if ( )` statement where you will input the condition that will trigger an action.

2. **Select Sensor Field:** Use the dropdown labeled Sensors to select the relevant sensor field (e.g., temperature, pressure, etc.) that will be used in your condition.

3. **Choose the Field Operator:** Once you've chosen the sensor field, select an operator (such as =, !=, <, >, etc.) from the next dropdown.

4. **Input a Value:** After selecting the operator, input the value for the comparison (e.g., Level = 6).

5. **Add the Condition:** After defining the condition, click "add to condition" to apply it. You can add multiple conditions by using the AND or OR buttons to create complex, multi-layered conditions.

6. **Combine Complex Conditions:** You can combine simple conditions with logical operators.

This structure allows for a range of complex scenarios to be covered.

7.**Reset:** Click the reset button to clear all input and start over if needed.

**Note:** If a condition or part of it is incomplete or incorrect, certain buttons, such as "add to condition" or AND/OR, will be disabled to prevent invalid configurations.


### Step 2: Building Actions

1. **Define the Action:** In the second window, specify what action should be taken when the condition is met. Write the desired action (e.g., "send") in the designated text box.

2. **Confirm the Action:** Once the action is defined, confirm it by clicking the green check mark.

3. **Add More Actions:** If additional actions are needed, you can add them by clicking the add condition button and repeating the steps.

4. **Finalize:** Once all conditions and actions are set, click the finish create button to save the configuration.

### Example:

- **Condition:** If [4] Level = 6 AND Temperature > 30
- **Action:**  send to sensor 4 slow!.

This guide helps you utilize the system’s flexibility for creating both simple and complex conditions, ensuring you can handle any scenario your project requires!
