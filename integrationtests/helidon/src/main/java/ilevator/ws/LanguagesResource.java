package ilevator.ws;

import java.io.IOException;
import java.util.logging.Logger;

import org.glassfish.tyrus.core.coder.ToStringEncoder;

import jakarta.websocket.OnMessage;
import jakarta.websocket.Session;
import jakarta.websocket.server.ServerEndpoint;

@ServerEndpoint(value = "/languages", encoders = { ToStringEncoder.class }, subprotocols = { "sperethiel", "leetspeak",
		"corp", "or'zet" })
public class LanguagesResource {

	private final Logger logger = Logger.getLogger(this.getClass().getName());

	@OnMessage
	public void onMessage(Session session, String message) {
		try {
			session.getBasicRemote().sendText(message);
		} catch (IOException e) {
			logger.severe(e.getMessage());
		}
	}
}
