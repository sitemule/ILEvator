package ilevator.ws;

import java.util.Queue;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.function.Consumer;

public class ObservableQueue<E> implements Queue<E> {

	private final Queue<E> queue;
	private final CopyOnWriteArrayList<Consumer<E>> listeners = new CopyOnWriteArrayList<>();

	public ObservableQueue(Queue<E> queue) {
		this.queue = queue;
	}

	public void addListener(Consumer<E> listener) {
		listeners.add(listener);
	}

	public void removeListener(Consumer<E> listener) {
		listeners.remove(listener);
	}

	@Override
	public boolean offer(E e) {
		boolean result = queue.offer(e);
		if (result) {
			notifyListeners(e);
		}
		return result;
	}

	@Override
	public boolean add(E e) {
		boolean result = queue.add(e);
		notifyListeners(e);
		return result;
	}

	// Notify all listeners
	private void notifyListeners(E e) {
		for (Consumer<E> listener : listeners) {
			listener.accept(e);
		}
	}

	// Delegate all other Queue methods to the underlying queue

	@Override
	public E remove() {
		return queue.remove();
	}

	@Override
	public E poll() {
		return queue.poll();
	}

	@Override
	public E element() {
		return queue.element();
	}

	@Override
	public E peek() {
		return queue.peek();
	}

	@Override
	public int size() {
		return queue.size();
	}

	@Override
	public boolean isEmpty() {
		return queue.isEmpty();
	}

	@Override
	public boolean contains(Object o) {
		return queue.contains(o);
	}

	@Override
	public boolean remove(Object o) {
		return queue.remove(o);
	}

	@Override
	public boolean containsAll(java.util.Collection<?> c) {
		return queue.containsAll(c);
	}

	@Override
	public boolean addAll(java.util.Collection<? extends E> c) {
		return queue.addAll(c);
	}

	@Override
	public boolean removeAll(java.util.Collection<?> c) {
		return queue.removeAll(c);
	}

	@Override
	public boolean retainAll(java.util.Collection<?> c) {
		return queue.retainAll(c);
	}

	@Override
	public void clear() {
		queue.clear();
	}

	@Override
	public java.util.Iterator<E> iterator() {
		return queue.iterator();
	}

	@Override
	public Object[] toArray() {
		return queue.toArray();
	}

	@Override
	public <T> T[] toArray(T[] a) {
		return queue.toArray(a);
	}
}
