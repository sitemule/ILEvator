package ilevator.proxy.echo;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.ServerSocket;
import java.net.Socket;

public class EchoService {

	private ServerSocket serverSocket;

	public EchoService(int port) throws IOException {
		serverSocket = new ServerSocket(port);
	}

	public void start() throws Exception {
		System.out.println("Starting echo service ...");

		Socket clientConnection = null;

		while ((clientConnection = serverSocket.accept()) != null) {
			InputStream is = clientConnection.getInputStream();
			BufferedReader reader = new BufferedReader(new InputStreamReader(is));

			OutputStream os = clientConnection.getOutputStream();
			BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(os));

			char[] buffer = new char[4096];
			int charsRead = reader.read(buffer);
			while (charsRead != -1) {
				System.out.println("Data: " + new String(buffer));
				writer.write(buffer);

				charsRead = reader.read(buffer);
			}

			System.out.println("Closing client connection");
			writer.flush();
			writer.close();
			os.close();
			is.close();
		}

		System.out.println("Ending echo service");
	}
}
