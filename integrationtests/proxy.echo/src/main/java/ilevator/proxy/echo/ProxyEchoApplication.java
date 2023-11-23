package ilevator.proxy.echo;

import org.littleshoot.proxy.HttpProxyServer;
import org.littleshoot.proxy.impl.DefaultHttpProxyServer;

public class ProxyEchoApplication {
	public static void main(String[] args) throws Exception {
		HttpProxyServer server = DefaultHttpProxyServer.bootstrap().withAllowLocalOnly(false).withPort(35801).start();

		EchoService echoService = new EchoService(35802);
		echoService.start();

		server.stop();
	}
}
