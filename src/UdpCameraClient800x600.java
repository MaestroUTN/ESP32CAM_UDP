import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.net.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.io.*;
import javax.imageio.ImageIO;

public class UdpCameraClient800x600 {
    private static final int PORT = 4444;
    private static JLabel label;
    private static JFrame frame;
    
    // Dimensiones objetivo
    private static final int WIDTH = 800;
    private static final int HEIGHT = 600;

    public static void main(String[] args) {
        frame = new JFrame("ESP32-CAM Stream 800x600");
        label = new JLabel();
        
        // Configuramos el label para que tenga el tamaño exacto
        label.setPreferredSize(new Dimension(WIDTH, HEIGHT));
        
        frame.getContentPane().add(label, BorderLayout.CENTER);
        frame.pack(); // Ajusta la ventana al tamaño del label
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setLocationRelativeTo(null);
        frame.setVisible(true);

        try (DatagramSocket socket = new DatagramSocket(PORT)) {
            // Aumentamos el buffer de red a 4MB para manejar SVGA sin pérdida
            socket.setReceiveBufferSize(4 * 1024 * 1024); 
            byte[] buffer = new byte[2048]; 

            System.out.println("Escuchando en puerto " + PORT + " para 800x600...");

            while (true) {
                // 1. Recibir cabecera (4 bytes del tamaño total)
                DatagramPacket headerPacket = new DatagramPacket(buffer, 4);
                socket.receive(headerPacket);
                
                int expectedSize = ByteBuffer.wrap(headerPacket.getData())
                                             .order(ByteOrder.LITTLE_ENDIAN)
                                             .getInt();

                // Validar que el tamaño sea coherente para un JPEG SVGA (usualmente 30KB - 180KB)
                if (expectedSize > 1000 && expectedSize < 500000) {
                    ByteArrayOutputStream imageBuffer = new ByteArrayOutputStream();
                    
                    // Tiempo de espera para no quedarse bloqueado en un frame incompleto
                    socket.setSoTimeout(1000); 

                    try {
                        while (imageBuffer.size() < expectedSize) {
                            DatagramPacket dataPacket = new DatagramPacket(buffer, buffer.length);
                            socket.receive(dataPacket);
                            imageBuffer.write(dataPacket.getData(), 0, dataPacket.getLength());
                        }
                        
                        byte[] imageBytes = imageBuffer.toByteArray();
                        SwingUtilities.invokeLater(() -> displayImage(imageBytes));
                        
                    } catch (SocketTimeoutException e) {
                        System.err.println("Frame descartado por pérdida de paquetes.");
                    }
                }
                socket.setSoTimeout(0); // Quitar timeout para esperar la siguiente cabecera
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void displayImage(byte[] data) {
        try {
            InputStream is = new ByteArrayInputStream(data);
            BufferedImage img = ImageIO.read(is);
            if (img != null) {
                // Mostramos la imagen directamente (ya viene en 800x600 desde el ESP32)
                label.setIcon(new ImageIcon(img));
            }
        } catch (IOException e) {
            // Error de decodificación silencioso
        }
    }
}