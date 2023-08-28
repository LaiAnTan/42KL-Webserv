class A():
	def __init__(self):
		self.x = 1
		
class B():
	def __init__(self, A: A):
		self.store = A

	def change(self):
		self.store.x = 69

a = A()
# b = a
# b.x = 10

b = B(a)
b.change()
print(a.x)