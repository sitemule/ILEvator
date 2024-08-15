package ilevator.ws;

import java.util.LinkedList;

import jakarta.enterprise.context.ApplicationScoped;
import jakarta.enterprise.inject.Produces;

@ApplicationScoped
public class MessageQueueProvider {

	private final ObservableQueue<String> messageQueue = new ObservableQueue<>(new LinkedList<>());

	@Produces
	public ObservableQueue<String> provideMessageQueue() {
		return messageQueue;
	}
}
