package ilevator.ws;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.eclipse.microprofile.config.inject.ConfigProperty;
import org.glassfish.tyrus.core.coder.InputStreamDecoder;
import org.glassfish.tyrus.core.coder.ToStringEncoder;

import jakarta.inject.Inject;
import jakarta.websocket.OnClose;
import jakarta.websocket.OnMessage;
import jakarta.websocket.OnOpen;
import jakarta.websocket.Session;
import jakarta.websocket.server.ServerEndpoint;

@ServerEndpoint(value = "/uploadws", encoders = { ToStringEncoder.class }, decoders = { InputStreamDecoder.class })
public class UploadEndpoint {
	private final Logger logger = Logger.getLogger(this.getClass().getName());

	@Inject
	@ConfigProperty(name = "ilevator.upload.dir")
	String uploadDirectory;

	@OnOpen
	public void onOpen(Session session) {
		logger.info("New session " + session.getId());
	}

	@OnClose
	public void onClose(Session session) {
		logger.info("Session removed " + session.getId());
	}

	@OnMessage
	public String receiveMessage(InputStream message) {
		java.nio.file.Path path = Paths.get(uploadDirectory, "upload-" + System.currentTimeMillis());

		try {
			Files.copy(message, path, StandardCopyOption.REPLACE_EXISTING);
			return path.getFileName().toString();
		} catch (IOException e) {
			logger.log(Level.SEVERE, "Error uploading file.", e);
			return e.getMessage();
		}
	}

}
