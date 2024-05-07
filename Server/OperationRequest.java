package Server;

/**
 * OperationRequest class is used to store the request details from the client.
 * It contains all possible fields that can be used in the request.
 * It has getter and setter methods for all the fields.
 */
public class OperationRequest {
    private Operation operationType;//operation type of the request
    private int requestId;//request id of the request
    private String pathname;//pathname of the file
    private String pathname1;//new pathname of the file used in move operation
    private int byteOffset;//byte offset used in write, delete and overwrite operations
    private int byteCount;//byte count used in delete operation
    private byte[] data;//data used in write and overwrite operations
    private int monitorTime;//monitoring time used in monitor operation
    private int clientPort;//client port
    private String clientAddress;//client address
    private long timestamp;//timestamp of the update operation
    //getter and setter methods
    public Operation getOperationType() {
        return operationType;
    }
    public void setOperationType(Operation operationType) {
        this.operationType = operationType;
    }
    public int getRequestId() {
        return requestId;
    }
    public void setRequestId(int requestId) {
        this.requestId = requestId;
    }
    public String getPathname() {
        return pathname;
    }
    public void setPathname(String pathname) {
        this.pathname = pathname;
    }
    public String getPathname1() {
        return pathname1;
    }
    public void setPathname1(String pathname1) {
        this.pathname1 = pathname1;
    }
    public int getByteOffset() {
        return byteOffset;
    }
    public void setByteOffset(int byteOffset) {
        this.byteOffset = byteOffset;
    }
    public int getByteCount() {
        return byteCount;
    }
    public void setByteCount(int byteCount) {
        this.byteCount = byteCount;
    }
    public byte[] getData() {
        return data;
    }
    public void setData(byte[] data) {
        this.data = data;
    }
    public int getMonitorTime() {
        return monitorTime;
    }
    public void setMonitorTime(int monitorTime) {
        this.monitorTime = monitorTime;
    }
    public int getClientPort() {
        return clientPort;
    }
    public void setClientPort(int clientPort) {
        this.clientPort = clientPort;
    }
    public String getClientAddress() {
        return clientAddress;
    }
    public void setClientAddress(String clientAddress) {
        this.clientAddress = clientAddress;
    }

    public void setTimestamp(long time) {
        this.timestamp = time;
    }
    public long getTimestamp() {
        return timestamp;
    }
}
