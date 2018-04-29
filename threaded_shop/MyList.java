//Daniel Ehrlich


import java.util.NoSuchElementException;
import java.util.Iterator;

public class MyList<E> implements Iterable<E>, Cloneable{
	private MyNode<E> n;
	public int size;
	
	public MyList(){
		n = null;
		size = 0;
	}
	
	public MyList(Iterable<E> iterable){
		n = null;
		size = 0;
		for(E e : iterable){
			if(n == null){
				push(e);
			}
			else{
				MyNode<E> temp = n;
				while(temp.getNext() != null)
					temp = temp.getNext();
				temp.setNext(new MyNode<E>(e,null));
				size++;
			}
		}
	}
	
	public Iterator<E> iterator(){
		return new MyNodeIterator<E>(n);
	}
	
	public MyList<E> clone(){
		MyList<E> temp = new MyList<E>();
		for(E e : this){
			temp.push(e);
		}
		MyList<E> clone = new MyList<E>();
		for(E e : temp){
			clone.push(e);
		}
		return clone;
	}
	
	public MyList<E> reverse(){
		MyNode<E> curr = n, prev = null, next = null;
		while(curr != null){
			next = curr.getNext();
			curr.setNext(prev);
			prev = curr;
			curr = next;
		}
		n = prev;
		return this;
	}
	public void push(E item){
		n = new MyNode<E>(item,n);
		size++;
	}
	public E pop(){
		if(size <= 0) throw new NoSuchElementException();
		else{
			E val = n.getV();
			n = n.getNext();
			size--;
			return val;
		}
	}
	public E peek(){
		if(size <= 0) throw new NoSuchElementException();
		return n.getV();
	}
	public String toString(){
		String output = "";
		if(this.size < 2)
			output = "[]";
		else{
			for(E e : this){
				output += "," + e;
			}
			output = "[" + output.substring(1) + "]";
		}
		return output;
	}
}
