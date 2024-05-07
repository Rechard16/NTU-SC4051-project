package Server;

import java.lang.reflect.Array;
import java.net.InetAddress;
import java.util.ArrayList;
/**
 * MonitorService class is used to manage all the file monitors.
 * It can register a new file monitor, remove all file monitors, clear expired file monitors, print all file monitors,
 * get all file monitors, get notified clients, move file monitor, and remove file monitor.
 */
public class MonitorService {
    private ArrayList<FileMonitor> fileMonitors = new ArrayList<>();
    private int monitorCount = 0;
    /**
     * RegisterMonitor method is used to register a new file monitor.
     * @param filePath the path of the file to be monitored
     * @param monitorTime the time interval to monitor the file
     * @param clientAddress the address of the client
     * @param clientPort the port of the client
     */
    public void RegisterMonitor(String filePath, int monitorTime, InetAddress clientAddress, int clientPort) {
        FileMonitor fileMonitor = new FileMonitor(filePath,  System.currentTimeMillis(),monitorTime,clientAddress,clientPort);
        fileMonitors.add(fileMonitor);
        monitorCount++;
    }
    /**
     * RemoveAllMonitor method is used to remove all file monitors.
     */
    public void removeAllMonitor() {
        fileMonitors.clear();
    }
    /**
     * ClearExpiredMonitor method is used to clear expired file monitors.
     */
    private void ClearExpiredMonitor() {
        for(int i=fileMonitors.size()-1;i>=0;i--) {
            if(fileMonitors.get(i).isExpired()) {
                fileMonitors.remove(i);
                monitorCount--;
            }
        }
    }
    /**
     * PrintAllMonitor method is used to print all file monitors.
     */
    public void printAllMonitor() {
        for (int i = 0; i < fileMonitors.size(); i++) {
            fileMonitors.get(i).print();
        }
    }
    /**
     * GetFileMonitors method is used to get all file monitors.
     * @return an array list of file monitors
     */
    public ArrayList<FileMonitor> getFileMonitors() {
        return fileMonitors;
    }
    /**
     * GetNotifiedClinet method is used to get notified clients by the file path.
     * @param filePath the path of the file to be monitored
     * @return an array list of file monitors
     */
    public ArrayList<FileMonitor> getNotifiedClinet(String filePath) {
        ClearExpiredMonitor();
        ArrayList<FileMonitor> result = new ArrayList<>();
        for (int i = 0; i < fileMonitors.size(); i++) {
            if (fileMonitors.get(i).getPath().equals(filePath)) {
                result.add(fileMonitors.get(i));
            }
        }
        return result;
    }
//    public void moveMonitor(String filePath, String newFilePath) {
//        for (int i = 0; i < fileMonitors.size(); i++) {
//            if (fileMonitors.get(i).getPath().equals(filePath)) {
//                fileMonitors.get(i).setPath(newFilePath);
//            }
//        }
//    }
    /**
     * RemoveMonitor method is used to remove a file monitor.
     * @param fileMonitor the file monitor to be removed
     */
    public void removeMonitor(FileMonitor fileMonitor) {
        fileMonitors.remove(fileMonitor);
        monitorCount--;
    }
}
