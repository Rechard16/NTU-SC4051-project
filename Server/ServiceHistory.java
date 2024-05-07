package Server;

import java.net.InetAddress;
import java.util.ArrayList;
/**
 * ServiceHistory class is used to store all the services that have been requested by the clients.
 * It contains a list of services.
 * It has methods to add a service to the list, find a service in the list, check if a service is in the list.
 */
public class ServiceHistory {
    private ArrayList<Service> services;//list of services
    /**
     * Constructor
     */
    public ServiceHistory() {
        services = new ArrayList<>();
    }
    /**
     * Add a service to the list
     * @param service
     */
    public void addService(Service service) {
        services.add(service);
    }
    /**
     * Find a service in the list
     * @param requestId
     * @param address
     * @param port
     * @return the service if it is in the list, null otherwise
     */
    public Service findService(int requestId, InetAddress address, int port) {
        for (Service service : services) {
            if (service.getRequestId() == requestId && service.getClientAddress().equals(address) && service.getClientPort() == port){
                return service;
            }
        }
        return null;
    }

    /**
     * Check if a service is in the list
     * @param requestId the request id of the service
     * @param address the address of the client
     * @param port the port of the client
     * @return true if the service is in the list, false otherwise
     */
    public boolean contain(int requestId, InetAddress address, int port) {
        for (Service service : services) {
            if (service.getRequestId() == requestId && service.getClientAddress().equals(address) && service.getClientPort() == port){
                return true;
            }
        }
        return false;
    }
    /**
     * Check if a service is in the list
     * @param service the service to check
     * @return true if the service is in the list, false otherwise
     */
    public boolean contain(Service service) {
        for (Service s : services) {
            if (s.equals(service)) {
                return true;
            }
        }
        return false;
    }

}
