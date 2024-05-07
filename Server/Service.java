package Server;

import java.net.InetAddress;

/**
 * Service class is used to store a single request information
 *
 */

public class Service {
    private final int requestId;// request id
    private final InetAddress clientAddress;// client address
    private final int clientPort;// client port
    private boolean isProcessed;//whether the request is processed
    private boolean isSuccessful;//whether the request is successful
    private byte[] responseMessage;//response message
    /**
     * Constructor
     * @param requestId request id
     * @param clientAddress client address
     * @param clientPort client port
     */
    public Service(int requestId, InetAddress clientAddress, int clientPort) {
        this.requestId = requestId;
        this.clientAddress = clientAddress;
        this.clientPort = clientPort;
        this.isProcessed = false;
        this.isSuccessful = false;

    }
    //getters and setters
    public int getRequestId() {
        return requestId;
    }
    public InetAddress getClientAddress() {
        return clientAddress;
    }
    public int getClientPort() {
        return clientPort;
    }
    public boolean isProcessed() {
        return isProcessed;
    }
    public boolean isSuccessful() {
        return isSuccessful;
    }
    public void setProcessed(boolean processed) {
        isProcessed = processed;
    }
    public void setSuccessful(boolean successful) {
        isSuccessful = successful;
    }

    public byte[] getResponseMessage() {
        return responseMessage;
    }
    public void setResponseMessage(byte[] responseMessage) {
        this.responseMessage = responseMessage;
    }
}
