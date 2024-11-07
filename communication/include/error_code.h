#ifndef ERRORCODE_H
#define ERRORCODE_H

enum class ErrorCode {
    SUCCESS = 0,                 
    SOCKET_FAILED = -1,          
    CONNECTION_FAILED = -2,      
    BIND_FAILED = -3,            
    LISTEN_FAILED = -4,          
    ACCEPT_FAILED = -5,          
    SEND_FAILED = -6,            
    RECEIVE_FAILED = -7,         
    CLOSE_FAILED = -8,           
    DISCONNECT_FAILED = -9,      
    INVALID_CLIENT_ID = -10,     
    CALLBACK_ERROR = -11,        
    SOCKET_INTERFACE_ERROR = -12,
    INVALID_DATA_SIZE = -13,     
    INVALID_DATA = -14,          
    INVALID_ID = -15             
};

// Function to convert ErrorCode to string
inline const char* toString(ErrorCode error) {
    switch (error) {
        case ErrorCode::SUCCESS: return "SUCCESS";
        case ErrorCode::SOCKET_FAILED: return "SOCKET_FAILED";
        case ErrorCode::CONNECTION_FAILED: return "CONNECTION_FAILED";
        case ErrorCode::SEND_FAILED: return "SEND_FAILED";
        case ErrorCode::RECEIVE_FAILED: return "RECEIVE_FAILED";
        case ErrorCode::CLOSE_FAILED: return "CLOSE_FAILED";
        case ErrorCode::CALLBACK_ERROR: return "CALLBACK_ERROR";
        case ErrorCode::SOCKET_INTERFACE_ERROR: return "SOCKET_INTERFACE_ERROR";
        case ErrorCode::BIND_FAILED: return "BIND_FAILED";
        case ErrorCode::LISTEN_FAILED: return "LISTEN_FAILED";
        case ErrorCode::ACCEPT_FAILED: return "ACCEPT_FAILED";
        case ErrorCode::INVALID_CLIENT_ID: return "INVALID_CLIENT_ID";
        case ErrorCode::DISCONNECT_FAILED: return "DISCONNECT_FAILED";
        case ErrorCode::INVALID_DATA_SIZE: return "INVALID_DATA_SIZE";
        case ErrorCode::INVALID_DATA: return "INVALID_DATA";
        case ErrorCode::INVALID_ID: return "INVALID_ID";
        default: return "UNKNOWN_ERROR";
    }
}

#endif // ERRORCODE_H
