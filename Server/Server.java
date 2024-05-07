package Server;

import java.io.IOException;
import java.net.*;
import java.nio.ByteBuffer;
import java.util.ArrayList;

import static Server.Unmarshalling.unmarshall;
/**
 * Server class is the main class of the server side. It listens to the client requests and sends the response back to the client.
 * It also handles the monitor requests and sends the monitor message to the clients.
 * It uses the FileService class to perform the file operations and MonitorService class to handle the monitor requests.
 * It uses the PacketDropSimulator class to simulate the packet drop.
 * It uses the ServiceHistory class to keep track of the services.
 */
public class Server {
    private static FileService fileService;// file service object
    private static MonitorService monitorService;// monitor service object
    private final static byte success = 1, fail = 0,monitor = 2;// response message type
    private static ServiceHistory serviceHistory;// service history object
    private static double dropRate = 0.07;// packet drop rate

    /**
     * Main method of the server side. It listens to the client requests and sends the response back to the client.
     * It also handles the monitor requests and sends the monitor message to the clients.
     */

    public static void main(String[] args) {
        //print the server IP address
        try {
            System.out.println("Server IP Address: " + InetAddress.getLocalHost().getHostAddress());
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }
        fileService = new FileService();//initialize file service
        monitorService = new MonitorService();//initialize monitor service
        serviceHistory = new ServiceHistory();//initialize service history
        PacketDropSimulator packetDropSimulator = new PacketDropSimulator(dropRate);//initialize packet drop simulator
        DatagramSocket socket = null;//socket object
        try {
            socket = new DatagramSocket(12345);//create socket
            byte[] buffer = new byte[2048];//buffer to store the request
            while (true) {//listen to the client requests
                DatagramPacket request = new DatagramPacket(buffer, buffer.length);//create packet to store the request
                socket.receive(request);//receive the request
                    if (!packetDropSimulator.drop()) {//simulate packet drop
                        System.out.println("Received request from " + request.getAddress());
                        handelRequest(request, socket);//handle the request
                    } else {
                        System.out.println("Dropped request from " + request.getAddress());
                    }

            }
        } catch (SocketException e) {//catch socket exception
            e.printStackTrace();
            System.out.println("Socket error");
        } catch (IOException e) {//catch IO exception
            e.printStackTrace();
            System.out.println("IO error");
        }
    }
    private static void handelRequest(DatagramPacket request, DatagramSocket socket)  {
        byte[] data = request.getData();//get the request data
        int clientPort = request.getPort();//get the client port
        InetAddress clientAddress = request.getAddress();//get the client address
        ArrayList<FileMonitor> fileMonitors =new ArrayList<FileMonitor>();//array list of file monitors which will be notified
        ByteBuffer byteBuffer = ByteBuffer.wrap(data);//wrap the data in byte buffer
        OperationRequest operationRequest = unmarshall(byteBuffer);//unmarshall the data
        int requestId = operationRequest.getRequestId();//get the request id
        Service service = null;//service object
        if(serviceHistory.contain(requestId,clientAddress,clientPort)) {//check if the service is already in the service history
            service = serviceHistory.findService(requestId, clientAddress, clientPort);
        }
        if(service != null){//service is already in the service history
            if(!(operationRequest.getOperationType() == Operation.UPDATE || operationRequest.getOperationType() == Operation.OVERWRITE))
                //check if the operation is update or overwrite, they are idempotent operations
            {
                if (service.isProcessed()) {//check if the non-idempotent service is already processed
                    System.out.println("Request already processed");
                    sendResponse(socket, clientAddress, clientPort, service.isSuccessful(), requestId, service.getResponseMessage());//resend the response
                    return;
                }
            }
        }else {//service is not in the service history
            service = new Service(requestId, clientAddress, clientPort);//create new service
            serviceHistory.addService(service);//add the service to the service history
        }
        boolean monitorFlag = false;//monitor flag check whether monitor message should be sent or not
        boolean successFlag = false;//success flag check whether the operation is successful or not
        byte[] responseMessage = null;//response message
        byte[] monitorMessage = null;//monitor message
        byte[][] errorMessage =new byte[1][];//error message
        byte[] notice = null;//notice message for monitor
        byte[] updatedData=null;//updated data

        switch(operationRequest.getOperationType()) {
//            abandon the read operation, as it is not required
//            case READ:
//                try {
//                    byte[] readData = fileService.read(operationRequest.getPathname(), operationRequest.getByteOffset(), operationRequest.getByteCount());
//                    successFlag = true;
//                    responseMessage = readData;
//                    service.setProcessed(true);
//                    service.setSuccessful(true);
//                } catch (FileNotFoundException e) {
//                    service.setProcessed(true);
//                    e.printStackTrace();
//                }
//                break;
            case WRITE://write operation
                fileService.write(operationRequest.getPathname(), operationRequest.getByteOffset(), operationRequest.getData(),errorMessage);
                successFlag = true;//set success flag to true
                monitorFlag = true;//set monitor flag to true
                //System.out.println("Error message: "+ Arrays.toString(errorMessage));
                if(errorMessage[0]!=null){//check if there is any error message
                    System.out.println("Error in writing file");
                    responseMessage = errorMessage[0];//set the error message
                    service.setProcessed(true);//set the service processed
                    service.setSuccessful(false);//set the service successful to false
                    service.setResponseMessage(responseMessage);//set the response message
                    break;
                }else {
                    notice = "File wrote\n".getBytes();//set the notice message for monitor
                    updatedData = fileService.readAll(operationRequest.getPathname(), errorMessage);//set the updated data
                    service.setProcessed(true);
                    service.setSuccessful(true);
                    responseMessage = "File wrote successfully.\n".getBytes();
                    service.setResponseMessage(responseMessage);
                    if(errorMessage[0]!=null){//check if there is any error message
                        monitorFlag = false;
                        break;
                    }else{
                    monitorMessage = new byte[notice.length + updatedData.length];
                    System.arraycopy(notice, 0, monitorMessage, 0, notice.length);
                    System.arraycopy(updatedData, 0, monitorMessage, notice.length, updatedData.length);
                }
                }
                break;
            case DELETE:
                fileService.delete(operationRequest.getPathname(), operationRequest.getByteOffset(), operationRequest.getByteCount(),errorMessage);
                if(errorMessage[0]!=null){//check if there is any error message
                    responseMessage = errorMessage[0];
                    service.setProcessed(true);
                    service.setSuccessful(false);
                    service.setResponseMessage(responseMessage);
                    break;
                }else {
                    successFlag = true;//set success flag to true
                    monitorFlag = true;//set monitor flag to true
                    notice = "File deleted\n".getBytes();//set the notice message for monitor
                    updatedData = fileService.readAll(operationRequest.getPathname(), errorMessage);//set the updated data
                    service.setProcessed(true);
                    service.setSuccessful(true);
                    responseMessage = "File deleted successfully.\n".getBytes();
                    service.setResponseMessage(responseMessage);
                    if(errorMessage[0]!=null){//check if there is any error message
                        monitorFlag = false;
                        break;
                        }else {
                        monitorMessage = new byte[notice.length + updatedData.length];//set the monitor message
                        System.arraycopy(notice, 0, monitorMessage, 0, notice.length);//copy the notice message
                        System.arraycopy(updatedData, 0, monitorMessage, notice.length, updatedData.length);//copy the updated data
                    }
                }
                break;
            case MONITOR:
                monitorService.RegisterMonitor(operationRequest.getPathname(), operationRequest.getMonitorTime(),clientAddress,clientPort);//register the monitor
                successFlag = true;//set success flag to true
                service.setProcessed(true);//set the service processed
                service.setSuccessful(true);//set the service successful to true
                responseMessage = "Monitor registered successfully.\n".getBytes();//set the response message
                service.setResponseMessage(responseMessage);//set the response message
                break;
            case MOVE:
                fileService.move(operationRequest.getPathname(), operationRequest.getPathname1(),errorMessage);
                if(errorMessage[0]!=null){//check if there is any error message
                    responseMessage = errorMessage[0];
                    service.setProcessed(true);
                    service.setSuccessful(false);
                    service.setResponseMessage(responseMessage);
                }else {
                    successFlag = true;//set success flag to true
                    monitorFlag = true;//set monitor flag to true
                    String newPath = operationRequest.getPathname1();
                    String message = "File moved to " + newPath + "\n";// the monitor message
                    byte[] msg = message.getBytes();
                    monitorMessage = msg;
                    service.setProcessed(true);//set the service processed
                    service.setSuccessful(true);
                    responseMessage = "File moved successfully.\n".getBytes();
                    service.setResponseMessage(responseMessage);//set the response message
                }
                break;
            case UPDATE:
                boolean updateFlag = fileService.updateCheck(operationRequest.getPathname(), operationRequest.getTimestamp(),errorMessage);
                if(errorMessage[0]!=null){//check if there is any error message
                    responseMessage = errorMessage[0];
                    service.setProcessed(true);
                    service.setSuccessful(false);
                    service.setResponseMessage(responseMessage);
                    break;
                }else {
                    successFlag = true;//set success flag to true
                    byte[] time = ByteBuffer.allocate(Long.BYTES).putLong(System.currentTimeMillis()).array();//get the current time

                    System.out.println("Time: " + System.currentTimeMillis());//print the current time
                    //responseMessage = updateFlag+time+(data length+data, if updateFlag is true)
                    if (updateFlag) {//check if the file need updated
                        updatedData = fileService.readAll(operationRequest.getPathname(), errorMessage);//read full file and add its length and data to response message
                        if(errorMessage[0]!=null){//check if there is any error message
                            responseMessage = errorMessage[0];
                            service.setProcessed(true);
                            service.setSuccessful(false);
                            service.setResponseMessage(responseMessage);
                            System.out.println("Error in reading file");
                            break;
                        }else{//no error message
                            byte[] length = ByteBuffer.allocate(Integer.BYTES).putInt(updatedData.length).array();//get the length of the updated data
                            responseMessage = new byte[1 + Long.BYTES + Integer.BYTES + updatedData.length];
                            responseMessage[0] = 1;//update flag
                            successFlag = true;
                            System.arraycopy(time, 0, responseMessage, 1, Long.BYTES);//copy the time to response message
                            System.arraycopy(length, 0, responseMessage, 1 + Long.BYTES, Integer.BYTES);//copy the length to response message
                            System.arraycopy(updatedData, 0, responseMessage, 1 + Long.BYTES + Integer.BYTES, updatedData.length);//copy the updated data to response message
                            service.setProcessed(true);//set the service processed
                            service.setSuccessful(true);//set the service successful to true
                            service.setResponseMessage(responseMessage);//set the response message
                            System.out.println("File need updated");
                        }
                    } else {//file not need updated
                        responseMessage = new byte[1 + Long.BYTES];//set the response message
                        responseMessage[0] = 0;//update flag
                        successFlag = true;//set success flag to true
                        System.arraycopy(time, 0, responseMessage, 1, Long.BYTES);//copy the time to response message
                        service.setProcessed(true);
                        service.setSuccessful(true);
                        service.setResponseMessage(responseMessage);//set the response message
                        System.out.println("File not need updated");
                    }
                }
                break;
            case OVERWRITE:
                fileService.overwrite(operationRequest.getPathname(), operationRequest.getByteOffset(), operationRequest.getData(),errorMessage);
                successFlag = true;
                monitorFlag = true;
                //System.out.println("Error message: "+ Arrays.toString(errorMessage));
                if(errorMessage[0]!=null){//check if there is any error message
                    System.out.println("Error in overwriting file");
                    responseMessage = errorMessage[0];
                    service.setProcessed(true);
                    service.setSuccessful(false);
                    service.setResponseMessage(responseMessage);
                    break;
                }else {
                    notice = "File overwrote\n".getBytes();
                    updatedData = fileService.readAll(operationRequest.getPathname(), errorMessage);
                    service.setProcessed(true);
                    service.setSuccessful(true);
                    responseMessage = "File overwrote successfully.\n".getBytes();
                    service.setResponseMessage(responseMessage);
                    if(errorMessage[0]!=null){
                        monitorFlag = false;
                        break;
                    }else{
                        monitorMessage = new byte[notice.length + updatedData.length];
                        System.arraycopy(notice, 0, monitorMessage, 0, notice.length);
                        System.arraycopy(updatedData, 0, monitorMessage, notice.length, updatedData.length);
                    }
                }
                break;
            default:
                break;
        }
        service.setProcessed(true);//set the service processed
        service.setSuccessful(successFlag);//set the service successful
        service.setResponseMessage(responseMessage);//set the response message
        System.out.println("Response sent to " + request.getAddress());//print the response sent to the client
        if (responseMessage != null) {
            System.out.println("Response message: " + new String(responseMessage));
        }
        sendResponse(socket,request.getAddress(),clientPort,successFlag,requestId,responseMessage);//send the response to the client
        if(monitorFlag&&(monitorMessage!=null)){//send the monitor message to the client
            fileMonitors = monitorService.getNotifiedClinet(operationRequest.getPathname());
            System.out.println(fileMonitors.size() + " monitors found");
            sendMonitor(socket,fileMonitors,monitorMessage);
            monitorFlag = false;
        }


    }
    /**
     * sendResponse method is used to send the response to the client.
     * @param socket the socket object
     * @param address the address of the client
     * @param port the port of the client
     * @param sucFlag the success flag
     * @param requestId the request id
     * @param responseMessage the response message
     */
    private static void sendResponse(DatagramSocket socket,InetAddress address,int port,boolean sucFlag,int requestId,byte[] responseMessage){
        ByteBuffer byteBuffer = ByteBuffer.allocate(1+ Integer.BYTES+Integer.BYTES+responseMessage.length);
        byteBuffer.put(sucFlag ? success : fail);//success or fail
        byteBuffer.putInt(requestId);//request id
        byteBuffer.putInt(responseMessage.length);//response message length
        byteBuffer.put(responseMessage);//response message
        //print bytebuffer message in hex
//        for (byte b : byteBuffer.array()) {
//            System.out.print(String.format("%02X ", b));
//        }
        byteBuffer.flip();//flip the byte buffer to send


        byte[] response = byteBuffer.array();
        DatagramPacket responsePacket = new DatagramPacket(response, response.length, address, port);
        try {
            socket.send(responsePacket);
        } catch (IOException e) {
            //throw new RuntimeException(e);
            System.out.println("Error in sending response to client");
        }
        byteBuffer.clear();//clear the byte buffer
    }

    /**
     * sendMonitor method is used to send the monitor message to the clients.
     * @param socket the socket object
     * @param fileMonitors the array list of goal file monitors
     * @param monitorMessage the monitor message
     */
    private static void sendMonitor(DatagramSocket socket,ArrayList<FileMonitor> fileMonitors,byte[] monitorMessage){
        ByteBuffer byteBuffer = ByteBuffer.allocate(1+Integer.BYTES+Integer.BYTES+monitorMessage.length);
        byteBuffer.put(monitor);//monitor flag
        byteBuffer.putInt(0);//allocate requestID for monitor message
        byteBuffer.putInt(monitorMessage.length);//monitor message length
        byteBuffer.put(monitorMessage);//monitor message
        byteBuffer.flip();//flip the byte buffer to send
        byte[] response = byteBuffer.array();
         for(FileMonitor fileMonitor:fileMonitors){
             if(fileMonitor.isExpired()){//check if the monitor is expired
                 monitorService.removeMonitor(fileMonitor);
                 continue;
             }
            DatagramPacket responsePacket = new DatagramPacket(response, response.length, fileMonitor.getClientAddress(), fileMonitor.getClientPort());//create packet to send monitor message
            try {
                System.out.println("Monitor message sent to " + fileMonitor.getClientAddress());
                socket.send(responsePacket);
            } catch (IOException e) {
                //throw new RuntimeException(e);
                System.out.println("Error in sending monitor message to client");
            }
        }
        byteBuffer.clear();//clear the byte buffer
    }
}
