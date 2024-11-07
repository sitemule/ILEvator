package ilevator.ws;

import java.util.logging.Logger;

import org.glassfish.tyrus.core.coder.ReaderDecoder;
import org.glassfish.tyrus.core.coder.ToStringEncoder;

import jakarta.websocket.OnClose;
import jakarta.websocket.OnMessage;
import jakarta.websocket.OnOpen;
import jakarta.websocket.Session;
import jakarta.websocket.server.ServerEndpoint;

@ServerEndpoint(value = "/echo", encoders = { ToStringEncoder.class }, decoders = { ReaderDecoder.class })
public class EchoEndpoint {

	private final Logger logger = Logger.getLogger(this.getClass().getName());

	@OnOpen
	public void onOpen(Session session) {
		logger.info("New session " + session.getId());
	}

	@OnClose
	public void onClose(Session session) {
		logger.info("Session removed " + session.getId());
	}

	@OnMessage
	public String echo(String message) {
		return message;
	}

}
