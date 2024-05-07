package Server;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.file.Files;
import java.nio.file.Paths;
/*
    * FileService class
    * This class is used to read, write, delete, move, read all, check update, overwrite file
    * in the server.
 */
public class FileService {
    //Read method is disabled, it would not be invoked by the client as updateCheck and readAll is used instead
//    public byte[] read(String pathname, int byteOffset, int byteCount) throws FileNotFoundException {
//        if(Files.exists(Paths.get(pathname))) {
//            try (RandomAccessFile file = new RandomAccessFile(pathname, "r")) {
//                if (byteOffset > file.length()) {
//                    throw new IndexOutOfBoundsException("Byte offset is greater than file length");
//                }
//                file.seek(byteOffset);
//                byte[] data = new byte[byteCount];
//                int bytesRead = file.read(data);
//                if(bytesRead<byteCount){
//                    return Arrays.copyOf(data, bytesRead);
//                }
//                return data;
//
//            } catch (IOException e) {
//                return "Error reading file".getBytes();
//            }
//        }
//        else {
//            return "File not found".getBytes();
//        }
//    }
    /*
        * Write method
        * This method is used to write data to a file at a specific byte offset, if error occurs, such as byte offset is greater than file length,
        * the method will pass an error message.
        * @param pathname: the path of the file
        * @param byteOffset: the byte offset to write data
        * @param data: the data to write
        * @param err: the error message
     */
    public void write(String pathname, int byteOffset, byte[] data,byte[][] err) {
        System.out.println("Write file "+pathname+" at "+byteOffset+" with data "+new String(data)+"\n");
        if(Files.exists(Paths.get(pathname)))//check if file exists
        {

            try (RandomAccessFile file = new RandomAccessFile(pathname, "rw")) {
                if (byteOffset > file.length()) {//if byte offset is greater than file length, return error message
                    err[0]="Byte offset is greater than file length".getBytes();
                    return;
                }
                byte[] tail = new byte[(int) (file.length() - byteOffset)];//get the tail of the file after the byte offset
                file.seek(byteOffset);//move the file pointer to the byte offset
                file.read(tail);//read the tail of the file
                file.seek(byteOffset);//move the file pointer to the byte offset
                file.write(data);//write the data to the file
                file.write(tail);//write the tail of the file to the file
                err[0]=null;
                System.out.println("Write file success");
            } catch (IOException e) {//if IOException occurs, return error message
                e.printStackTrace();
                err[0]= "Error writing file".getBytes();
                System.out.println("Error writing file");
            }
        }
        else {//if file not found, return error message
            err[0]= "File not found".getBytes();
            System.out.println("File not found");
        }
    }
    /*
        * Delete method
        * This method is used to delete data from a file at a specific byte offset
        * @param pathname: the path of the file
        * @param byteOffset: the byte offset to delete data
        * @param delCount: the number of bytes to delete
        * @param err: the error message
     */
    public void delete(String pathname,int byteOffset,int delCount,byte[][] err) {
        System.out.println("Delete file "+pathname+" at "+byteOffset+" with count "+delCount+"\n");
        if(Files.exists(Paths.get(pathname)))//check if file exists
        {
            try (RandomAccessFile file = new RandomAccessFile(pathname, "rw")) {
                if (byteOffset > file.length()) {//if byte offset is greater than file length, return error message
                    err[0]="Byte offset is greater than file length".getBytes();
                    return;
                }
                int byteDel=(int)Math.min(file.length()-byteOffset,delCount);//get the number of bytes to delete
                if(byteDel<=0)return;//if no bytes to delete, return

                int tailStart=byteOffset+byteDel;//get the start position of the tail
                int tailLen=(int)file.length()-tailStart;//get the length of the tail
                byte[] tail = new byte[tailLen];//get the tail of the file

                file.seek(tailStart);//move the file pointer to the start position of the tail
                file.read(tail);//read the tail of the file
                file.seek(byteOffset);//move the file pointer to the byte offset
                file.write(tail);//write the tail of the file to the byte offset
                file.setLength(file.length()-byteDel);//set the length of the file
//                file.seek(0);
//                byte[] data = new byte[(int) file.length()];
//                file.read(data);
                err[0]=null;
            } catch (IOException e) {//if IOException occurs, return error message
                err[0]="Error deleting file".getBytes();
            }
        }
        else {//if file not found, return error message
            err[0]="File not found".getBytes();
        }
    }
    /*
        * Move method
        * This method is used to move a file to a new path
        * @param pathname: the path of the file
        * @param newpathname: the new path of the file
        * @param err: the error message
     */
    public void move(String pathname, String newpathname,byte[][] err) {
        System.out.println("Move file "+pathname+" to "+newpathname+"\n");
        if(Files.exists(Paths.get(pathname)))//check if file exists
        {
            try {
                Files.move(Paths.get(pathname), Paths.get(newpathname));//move the file to the new path
            } catch (IOException e) {//if IOException occurs, return error message
                err[0]="Error moving file".getBytes();
            }
        }
        else {//if file not found, return error message
            err[0]="File not found".getBytes();
        }
    }
    public byte[] readAll(String pathname,byte[][] err) {
        if(Files.exists(Paths.get(pathname)))//check if file exists
        {
            try (RandomAccessFile file = new RandomAccessFile(pathname, "r")) {
                byte[] data = new byte[(int) file.length()];//read all data from the file
                file.read(data);//read the data
                err[0]=null;
                return data;
            } catch (IOException e) {//if IOException occurs, return error message
                err[0]="Error reading file".getBytes();
                return null;
            }
        }
        else {//if file not found, return error message
            err[0]="File not found".getBytes();
            return null;
        }
    }
    /*
        * UpdateCheck method
        * This method is used to check if a file needs to be updated
        * @param pathname: the path of the file
        * @param lastUpdate: the last update time of the file
        * @param err: the error message
        * @return: true if the file needs to be updated, false otherwise
     */
    public boolean updateCheck(String pathname,long lastUpdate,byte[][] err) {
        System.out.println("Check file "+pathname+" update\n");
        if(Files.exists(Paths.get(pathname)))//check if file exists
        {
            try {
                long lastModified = Files.getLastModifiedTime(Paths.get(pathname)).toMillis();//get the last modified time of the file
                if(lastModified > lastUpdate) {//if the last modified time is greater than the last update time, return true
                    err[0]=null;
                    return true;
                }
                return false;
            } catch (IOException e) {//if IOException occurs, return error message
                err[0]="Error checking file update".getBytes();
                return false;
            }
        }
        else {//if file not found, return error message
            err[0]="File not found".getBytes();
            return false;
        }
    }
    /*
        * Overwrite method
        * This method is used to overwrite data to a file at a specific byte offset
        * @param pathname: the path of the file
        * @param byteOffset: the byte offset to overwrite data
        * @param data: the data to overwrite
        * @param err: the error message
     */
    public void overwrite(String pathname, int byteOffset, byte[] data,byte[][] err) {
        System.out.println("Overwrite file "+pathname+" at "+byteOffset+" with data "+new String(data)+"\n");
        if(Files.exists(Paths.get(pathname)))//check if file exists
        {

            try (RandomAccessFile file = new RandomAccessFile(pathname, "rw")) {
                if (byteOffset > file.length()) {//if byte offset is greater than file length, return error message
                    err[0]="Byte offset is greater than file length".getBytes();
                    return;
                }
                file.seek(byteOffset);//move the file pointer to the byte offset
                file.write(data);//write the data to the file
                err[0]=null;
                System.out.println("Overwrite file success");
            } catch (IOException e) {//if IOException occurs, return error message
                e.printStackTrace();
                err[0]= "Error overwriting file".getBytes();
                System.out.println("Error writing file");
            }
        }
        else {//if file not found, return error message
            err[0]= "File not found".getBytes();
            System.out.println("File not found");
        }
    }
}