/*************************************************************************
	> File Name: HelloWorld.java
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Tue 14 Jun 2016 08:57:43 AM CST
	> */
	import java.nio.ByteBuffer; 
	import java.io.IOException; 
import java.nio.ByteOrder;
	
	
public class HelloWorld{
      public static void main(String args[]){
          // 向控制台输出信息
              System.out.println("欢迎java01班的同学");
              onMessage();
             
      }
      public static void onMessage() {  
        //System.out.println("echoBinary: " + data);  
         ByteBuffer data = ByteBuffer.allocate(10);  
	        data.put((byte)0);  
	        data.putInt(0x12345678);  
	        data.asCharBuffer().put("12");
	       
        for (byte b : data.array()) {  
            System.out.print(Integer.toHexString(b & 0xFF).toUpperCase() + " ");  
        }  
        System.out.println("");
        System.out.println(Integer.toHexString(data.order(ByteOrder.BIG_ENDIAN).getInt(1)).toUpperCase() );
        System.out.println(Integer.toHexString(data.order(ByteOrder.LITTLE_ENDIAN).getInt(1)).toUpperCase() );
        
    } 
}

