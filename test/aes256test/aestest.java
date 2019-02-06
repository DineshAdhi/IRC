import java.io.*;
import java.util.*;
import javax.crypto.*;
import javax.crypto.spec.*;

public class aestest 
{
    private static String key = "qwertyuiopasdfghjklzxcvbnmqwerty";

    private static final String ALGORITHM = "AES/ECB/NOPADDING";

    public static byte[] encrypt(byte[] plainText) throws Exception
    {
        SecretKeySpec secretKey = new SecretKeySpec(key.getBytes(), "AES");
        Cipher cipher = Cipher.getInstance(ALGORITHM);
        cipher.init(Cipher.ENCRYPT_MODE, secretKey);

        return cipher.doFinal(plainText);
    }

    public static byte[] decrypt(byte[] cipherText) throws Exception
    {
        SecretKeySpec secretKey = new SecretKeySpec(key.getBytes(), "AES");
        Cipher cipher = Cipher.getInstance(ALGORITHM);
        cipher.init(Cipher.DECRYPT_MODE, secretKey);

        return cipher.doFinal(cipherText);
    }
    public static void main(String args[]) throws Exception
    {
        String str = "asdfasdfasdfasdf";
        byte[] data = str.getBytes();

        int rem = (16 - data.length % 16);
        
        System.out.println(data.length);
        System.out.println(rem);

        byte[] newdata = new byte[data.length + rem];

        for(int i=0; i<data.length; i++)
            newdata[i] = data[i];

        for(int i= data.length; i<rem; i++)
        {
            newdata[i] = (char) 0x00;
        }

        newdata[data.length + rem - 1] = -1;

        System.out.println(newdata.length);

        byte[] enc = encrypt(newdata);

        for(int i=0; i<enc.length; i++)
        {
            System.out.print((enc[i] & 0xff) + " ");
        }

        byte[] dec = decrypt(enc);

        System.out.println("\n\n" +  new String(dec) + "\n\n");

        for(int i=0; i<dec.length; i++)
        {
            System.out.print((dec[i] & 0xff) + " ");
        }
    }
}