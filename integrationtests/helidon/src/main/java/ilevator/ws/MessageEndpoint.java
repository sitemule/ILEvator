package ilevator.ws;

import java.util.Collections;
import java.util.HashSet;
import java.util.Set;
import java.util.logging.Logger;

import org.glassfish.tyrus.core.coder.ToStringEncoder;

import jakarta.annotation.PostConstruct;
import jakarta.inject.Inject;
import jakarta.websocket.OnClose;
import jakarta.websocket.OnOpen;
import jakarta.websocket.Session;
import jakarta.websocket.server.ServerEndpoint;

@ServerEndpoint(value = "/messagews", encoders = { ToStringEncoder.class })
public class MessageEndpoint {

	private final Logger logger = Logger.getLogger(this.getClass().getName());
	private final Set<Session> sessions = Collections.synchronizedSet(new HashSet<>());

	@Inject
	private ObservableQueue<String> messageQueue;

	@PostConstruct
	private void postConstruct() {
		messageQueue.addListener(message -> notifyAllClient());
	}

	@OnOpen
	public void onOpen(Session session) {
		logger.info("New session " + session.getId());
		sessions.add(session);
	}

	@OnClose
	public void onClose(Session session) {
		sessions.remove(session);
		logger.info("Session removed " + session.getId());
	}

	private void notifyAllClient() {
		logger.info("Notify all");

		synchronized (sessions) {
			while (!messageQueue.isEmpty()) {
				String message = messageQueue.poll();

				for (Session session : sessions) {
					if (session.isOpen()) {
						logger.info("Sending message to " + session.getId());
						session.getAsyncRemote().sendText(message);
					}
				}
			}
		}

	}

}
