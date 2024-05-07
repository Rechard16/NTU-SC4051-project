package Server;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
/**
 * Unmarshalling class is used to unmarshall the request from the client
 *
 */
public class Unmarshalling {
    /**
     * Unmarshall the request from the client
     * @param bf byte buffer
     * @return OperationRequest
     */
    public static OperationRequest unmarshall(ByteBuffer bf) {
        OperationRequest operationRequest = new OperationRequest();//create a new OperationRequest object
        byte operationCode = bf.get();//read the operation code
        operationRequest.setOperationType(Operation.values()[operationCode]);//set the operation type
        int requestId = bf.getInt();//read the request id
        operationRequest.setRequestId(requestId);//set the request id
        switch(operationCode) {
            case 0://read operation, disabled in the current version
                break;
            case 1://write
            case 6://overwrite
                int offset1=bf.getInt();//read the offset
                operationRequest.setByteOffset(offset1);
                String pathname0=readString(bf);//read the pathname
                operationRequest.setPathname(pathname0);
                String data=readString(bf);//read the writing data
                operationRequest.setData(data.getBytes());
                break;
            case 3://delete
                int offset=bf.getInt();//read the offset
                operationRequest.setByteOffset(offset);
                int count=bf.getInt();//read the byte count
                operationRequest.setByteCount(count);
                String pathname=readString(bf);//read the pathname
                operationRequest.setPathname(pathname);
                break;
            case 2://monitor
                int monitorTime=bf.getInt();//read the monitor duration
                operationRequest.setMonitorTime(monitorTime);
                String pathname1=readString(bf);//read the pathname
                operationRequest.setPathname(pathname1);
                break;
            case 4://move
                String pathname2=readString(bf);//read the source pathname
                operationRequest.setPathname(pathname2);
                String pathname3=readString(bf);//read the destination pathname
                operationRequest.setPathname1(pathname3);
                break;
            case 5://update
                long time=bf.getLong();//read the timestamp
                operationRequest.setTimestamp(time);
                String pathname4=readString(bf);//read the pathname
                operationRequest.setPathname(pathname4);
                break;
            default:
                System.out.println("Invalid request pack");
                return null;
        }
        return operationRequest;
    }
    /**
     * Read a string from the byte buffer
     * @param bf byte buffer
     * @return string
     */
    private static String readString(ByteBuffer bf) {//read from C++ client,char is 1 byte
        int length = bf.getInt();
        byte[] stringBytes = new byte[length];
        bf.get(stringBytes);
        return new String(stringBytes, StandardCharsets.UTF_8);
    }
}
