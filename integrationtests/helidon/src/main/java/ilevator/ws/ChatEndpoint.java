package ilevator.ws;

import java.util.Collections;
import java.util.HashSet;
import java.util.Set;
import java.util.logging.Logger;

import org.glassfish.tyrus.core.coder.ReaderDecoder;
import org.glassfish.tyrus.core.coder.ToStringEncoder;

import jakarta.websocket.OnClose;
import jakarta.websocket.OnMessage;
import jakarta.websocket.OnOpen;
import jakarta.websocket.Session;
import jakarta.websocket.server.ServerEndpoint;

@ServerEndpoint(value = "/chat", encoders = { ToStringEncoder.class }, decoders = {
		ReaderDecoder.class }, subprotocols = { "chat.rpgnextgen.com" })
public class ChatEndpoint {

	private final Logger logger = Logger.getLogger(this.getClass().getName());
	private final Set<Session> sessions = Collections.synchronizedSet(new HashSet<>());

	private final ChatMessageParser parser = new ChatMessageParser();

	@OnOpen
	public void onOpen(Session session) {
		logger.info("New session " + session.getId());
	}

	@OnClose
	public void onClose(Session session) {
		sessions.remove(session);
		logger.info("Session removed " + session.getId());
	}

	@OnMessage
	public void receiveMessage(Session session, String message) {

		try {
			ChatMessage cm = parser.toMessage(message);

			switch (cm.type) {
			case "join": {
				logger.info(session.getId() + " is joining");
				sessions.add(session);
			}
			case "leave": {
				logger.info(session.getId() + " is leaving");
				sessions.remove(session);
			}
			case "message": {
				notifyAllClients(cm.content, session.getId());
			}
			}
		} catch (Exception e) {
			logger.info("Message dropped. " + e.getMessage());
		}
	}

	private void notifyAllClients(String message, String currentSessionId) {
		logger.info("Notify all");

		synchronized (sessions) {
			for (Session session : sessions) {
				if (!session.getId().equals(currentSessionId) && session.isOpen()) {
					logger.info("Sending message to " + session.getId());
					session.getAsyncRemote().sendText(message);
				}
			}
		}

	}

	class ChatMessageParser {
		public ChatMessage toMessage(String payload) {
			if (payload.length() < 11)
				throw new InvalidMessageType();

			return new ChatMessage(payload.substring(0, 10), payload.substring(10));
		}
	}

	record ChatMessage(String type, String content) {
		public ChatMessage() {
			this(null, null);
		}
	}

	class InvalidMessageType extends RuntimeException {
		private static final long serialVersionUID = -1151720890082801535L;

		public InvalidMessageType() {
			super("Passed message type is invalid.");
		}

		public InvalidMessageType(String message) {
			super(message);
		}
	}
}
