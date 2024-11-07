#include "sensor.h"
using namespace std;

// C-tor initializes the id member variable.
Sensor::Sensor(int id, string name, string jsonFilePath) : id(id), name(name)
{
    parser = new PacketParser(jsonFilePath);
    std::vector<Field> tempFields = parser->getFields();

    for (auto field : tempFields) {
        if (field.type == "bit_field") 
            for (auto subField : parser->getBitFieldFields(field.name)) {
                GlobalProperties::controlLogger.logMessage(logger::LogLevel::DEBUG, subField.name + " : " + subField.type);
                fieldsMap[subField.name] = subField;
            }
        else {
            GlobalProperties::controlLogger.logMessage(logger::LogLevel::DEBUG, field.name + " : " + field.type);
            fieldsMap[field.name] = field;
        }  
    }
}

void Sensor::handleMessage(void *msg)
{
    parser->setBuffer(msg);

    for (auto field : fieldsMap) {
        string fieldName = field.first;
        GlobalProperties::controlLogger.logMessage(logger::LogLevel::DEBUG, "Processing field: " + fieldName);

        updateTrueRoots(fieldName, parser->getFieldValue(fieldName),
                        parser->getFieldType(field.second.type));
    }
}

//Updates the condition status according to the received field and returns the  list of the full conditions whose root is true
void Sensor::updateTrueRoots(string field, FieldValue value, FieldType type)
{
    // Update the field value in the sensor
    this->fields[field].first = value;

    // Evaluate each condition related to the field
    for (BasicCondition *bc : this->fields[field].second) {
        bool flag = false, prevStatus = bc->status;
        FieldValue bcValue = bc->value;
        OperatorTypes op = bc->operatorType;

        switch (type) {
            case FieldType::UNSIGNED_INT: {
                flag =
                    applyComparison(get<unsigned int>(value),
                                    get<unsigned int>(bcValue), op);
                break;
            }
            case FieldType::SIGNED_INT: {
                flag = applyComparison(get<int>(value),
                                       get<int>(bcValue), op);
                break;
            }
            case FieldType::CHAR_ARRAY: {
                flag = applyComparison(get<string>(value),
                                       get<string>(bcValue), op);
                break;
            }
            case FieldType::FLOAT_FIXED: {  
                flag = applyComparison(get<float>(value),
                                       get<float>(bcValue), op);
                break;
            }
            case FieldType::FLOAT_MANTISSA: {
                flag = applyComparison(get<float>(value),
                                       get<float>(bcValue), op);
                break;
            }
            case FieldType::BOOLEAN: {
                flag = applyComparison(get<bool>(value),
                                       get<bool>(bcValue), op);
                break;
            }
            case FieldType::DOUBLE: {
                flag = applyComparison(get<double>(value),
                                       get<double>(bcValue), op);
            }
            default: {
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
    GlobalProperties::controlLogger.logMessage(logger::LogLevel::DEBUG, "applyComparison");
    switch (op) {
        case OperatorTypes::e:
            return a == b;
        case OperatorTypes::ne:
            return a != b;
        case OperatorTypes::l:
            return a < b;
        case OperatorTypes::b:
            return a > b;
        case OperatorTypes::le:
            return a <= b;
        case OperatorTypes::be:
            return a >= b;
    }  
    return false;
}