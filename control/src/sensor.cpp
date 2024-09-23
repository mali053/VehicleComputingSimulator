#include "sensor.h"
using namespace std;

void Sensor::handleMessage(void *msg)
{
    parser->setBuffer(msg);

    for (auto field : fieldsMap) {
        string fieldName = field.first;
        GlobalProperties::controlLogger.logMessage(logger::LogLevel::DEBUG, "Processing field: " + fieldName);

        cout << "----------------\nfieldName: " << fieldName << endl;
        cout << "field type: " << field.second.type << endl;
        updateTrueRoots(fieldName, parser->getFieldValue(fieldName),
                        parser->getFieldType(field.second.type));
    }
}

//Updates the condition status according to the received field and returns the  list of the full conditions whose root is true
void Sensor::updateTrueRoots(string field, void *value, FieldType type)
{
    // Update the field value in the sensor
    this->fields[field].first = value;

    // Evaluate each condition related to the field
    for (BasicCondition *bc : this->fields[field].second) {
        bool flag = false, prevStatus = bc->status;
        void *bcValue = bc->value;
        OperatorTypes op = bc->operatorType;

        switch (type) {
            case FieldType::UNSIGNED_INT: {
                flag =
                    applyComparison(*static_cast<unsigned int *>(value),
                                    *static_cast<unsigned int *>(bcValue), op);
                break;
            }
            case FieldType::SIGNED_INT: {
                flag = applyComparison(*static_cast<int *>(value),
                                       *static_cast<int *>(bcValue), op);
                break;
            }
            case FieldType::CHAR_ARRAY: {
                flag = applyComparison(*static_cast<char **>(value),
                                       *static_cast<char **>(bcValue), op);
                break;
            }
            case FieldType::FLOAT_FIXED: {
                flag = applyComparison(*static_cast<float *>(value),
                                       *static_cast<float *>(bcValue), op);
                break;
            }
            case FieldType::FLOAT_MANTISSA: {
                flag = applyComparison(*static_cast<float *>(value),
                                       *static_cast<float *>(bcValue), op);
                break;
            }
            case FieldType::BIT_FIELD: {
                flag = applyComparison(*static_cast<int *>(value),
                                       *static_cast<int *>(bcValue), op);
                break;
            }
            case FieldType::DOUBLE: {
                flag = applyComparison(*static_cast<double *>(value),
                                       *static_cast<double *>(bcValue), op);
            }
            default: {
                cout << "DEFAULT" << endl;
                GlobalProperties::controlLogger.logMessage(logger::LogLevel::ERROR, "Invalid FieldType encountered");
                break;
            }
        }

        // Update the condition's status
        bc->status = flag;

        // If the condition's status has changed
        if (flag != prevStatus) {
            GlobalProperties::controlLogger.logMessage(logger::LogLevel::DEBUG, "Condition status changed for field: " + field);

            // Update parent conditions and check if the root condition is true
            for (Node *parent : bc->parents) {
                (bc->status) ? parent->countTrueConditions++
                             : parent->countTrueConditions--;
                parent->updateTree();
                GlobalProperties::controlLogger.logMessage(logger::LogLevel::INFO, "Updated parent tree for field: " + field);
            }
        }
    }
}

// Function to apply the correct operator
template <typename T>
bool Sensor::applyComparison(T a, T b, OperatorTypes op)
{
    cout << "applyComparison" << endl;
    cout << "a: " << a << ", b: " << b << endl;
    switch (op) {
        case OperatorTypes::e:
            return a == b;
        case OperatorTypes::ne:
            return a != b;
        case OperatorTypes::l: {
            cout << "(" << a << " < " << b << ") ? " << ((a < b) ? "V" : "X") << endl;
            return a < b;
        }
        case OperatorTypes::b: {
            cout << "(" << a << " > " << b << ") ? " << ((a > b) ? "V" : "X") << endl;
            return a > b;
        }
        case OperatorTypes::le:
            return a <= b;
        case OperatorTypes::be:
            return a >= b;
    }
    return false;
}