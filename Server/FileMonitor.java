package Server;

import java.net.InetAddress;
/**
 * FileMonitor class is used to store the information of a single file monitoring
 */
public class FileMonitor {
    private String path;//path of the file
    private long startTime;//start time of monitoring in milliseconds
    private int monitorTime;//duration of monitoring time in seconds
    private InetAddress clientAddress;//client address
    private int clientPort;//client port
    private long ExpiredTime;//expired time of monitoring
    /**
        * Constructor of FileMonitor class
        * @param path: path of the file
        * @param startTime: start time of monitoring in milliseconds
        * @param monitorTime: duration of monitoring time in seconds
        * @param clientAddress: client address
        * @param clientPort: client port
     */
    public FileMonitor(String path, long startTime, int monitorTime, InetAddress clientAddress, int clientPort) {
        this.path = path;
        this.startTime = startTime;
        this.monitorTime = monitorTime;
        this.clientAddress = clientAddress;
        this.clientPort = clientPort;
        this.ExpiredTime = startTime + monitorTime* 1000L;
    }
    //getter and setter methods
    public String getPath() {
        return path;
    }
    public void setPath(String path) {
        this.path = path;
    }
    public long getStartTime() {
        return startTime;
    }
    public int getMonitorTime() {
        return monitorTime;
    }
    public InetAddress getClientAddress() {
        return clientAddress;
    }
    public int getClientPort() {
        return clientPort;
    }
    public void setMonitorTime(int monitorTime) {
        this.monitorTime = monitorTime;
    }
    public long getExpiredTime() {
        return ExpiredTime;
    }
    /**
        * Check if the monitoring is expired
        * @return: true if the monitoring is expired, false otherwise
     */
    public boolean isExpired() {
        return System.currentTimeMillis() > ExpiredTime;
    }
    //print the information of the file when debugging
    public void print() {
        System.out.println("Path: " + path);
        System.out.println("Last Update: " + startTime);
        System.out.println("Monitor Time: " + monitorTime);
    }
    //clear the information of the file
    public void clear() {
        path = null;
        startTime = 0;
        monitorTime = 0;
        clientAddress = null;
        clientPort = 0;
        ExpiredTime = 0;
    }
}
