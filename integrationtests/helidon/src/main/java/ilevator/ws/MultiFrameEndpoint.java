package ilevator.ws;

import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.commons.lang3.StringUtils;
import org.glassfish.tyrus.core.coder.ReaderDecoder;
import org.glassfish.tyrus.core.coder.ToStringEncoder;

import jakarta.websocket.OnClose;
import jakarta.websocket.OnMessage;
import jakarta.websocket.OnOpen;
import jakarta.websocket.Session;
import jakarta.websocket.server.ServerEndpoint;

@ServerEndpoint(value = "/multiframe", encoders = { ToStringEncoder.class }, decoders = { ReaderDecoder.class })
public class MultiFrameEndpoint {

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
	public void receiveMessage(Session session, String message) {
		if (StringUtils.isNotBlank(message))
			logger.info("Received message: " + message);

		try {
			session.getBasicRemote().sendText("And some things that should not have been forgotten were lost. ", false);
			session.getBasicRemote().sendText("History became legend. Legend became myth. ", false);
			session.getBasicRemote().sendText("And for two and a half thousand years, ", false);
			session.getBasicRemote().sendText("the ring passed out of all knowledge.", false);
			session.getBasicRemote().sendText(" -- Galadriel", true);
		} catch (Exception e) {
			logger.log(Level.SEVERE, "Could not send multi frame message to client. ", e);
		}
	}

}
