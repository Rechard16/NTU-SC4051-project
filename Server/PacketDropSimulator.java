package Server;
import java.util.Random;
/**
 * PacketDropSimulator class is used to simulate the packet drop.
 */
public class PacketDropSimulator {
    private static double dropRate = 0.0;
    private Random random;
    /**
     * Constructor of PacketDropSimulator class
     * @param dropRate: drop rate of the packet
     */
    public PacketDropSimulator(double dropRate) {
        this.dropRate = dropRate;
        random = new Random();
    }
    /**
     * Drop method is used to simulate the packet drop.
     * @return: true if the packet is dropped, false otherwise
     */
    public boolean drop() {
        return random.nextDouble() < dropRate;
    }
}
