
#include <iostream>
#include <string>
#include <ctime>
using namespace std;

struct Product {
    int id;
    string name;
    string category;
    double price;
    int quantity;
    Product* next;
};

class ProductCatalog {
private:
    Product* head;

public:
    ProductCatalog() {
        head = nullptr;
    }

    void addProduct(int id, const string& name, const string& category, double price, int quantity) {
        if (searchById(id) != nullptr) {
            cout << "Error: ID already exists." << '\n';
            return;
        }

        Product* newP = new Product();
        newP->id = id;
        newP->name = name;
        newP->category = category;
        newP->price = price;
        newP->quantity = quantity;
        newP->next = nullptr;

        if (head == nullptr) {
            head = newP;
        } else {
            Product* temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newP;
        }
    }

    Product* searchById(int id) {
        Product* temp = head;
        while (temp != nullptr) {
            if (temp->id == id) {
                return temp;
            }
            temp = temp->next;
        }
        return nullptr;
    }

    void updateProduct(int id, double newPrice, int newQuantity) {
        Product* p = searchById(id);
        if (p == nullptr) {
            cout << "Product not found." << '\n';
            return;
        }
        p->price = newPrice;
        p->quantity = newQuantity;
        cout << "Product updated." << '\n';
    }

    void deleteProduct(int id) {
        if (head == nullptr) {
            cout << "Catalog is empty." << '\n';
            return;
        }

        if (head->id == id) {
            Product* del = head;
            head = head->next;
            delete del;
            cout << "Product deleted." << '\n';
            return;
        }

        Product* temp = head;
        Product* prev = nullptr;

        while (temp != nullptr && temp->id != id) {
            prev = temp;
            temp = temp->next;
        }

        if (temp == nullptr) {
            cout << "Product not found." << '\n';
            return;
        }

        prev->next = temp->next;
        delete temp;
        cout << "Product deleted." << '\n';
    }

    void display() {
        if (head == nullptr) {
            cout << "Catalog is empty." << '\n';
            return;
        }

        cout << "ID\tName\t\tCategory\tPrice\tQty" << '\n';
        cout << "-----------------------------------------------" << '\n';

        Product* temp = head;
        while (temp != nullptr) {
            cout << temp->id << "\t"
                 << temp->name << "\t\t"
                 << temp->category << "\t\t"
                 << temp->price << "\t"
                 << temp->quantity << '\n';
            temp = temp->next;
        }
    }

    bool reduceQuantity(int id, int qty) {
        Product* p = searchById(id);
        if (p == nullptr) {
            return false;
        }
        if (p->quantity >= qty) {
            p->quantity -= qty;
            return true;
        }
        return false;
    }

    int getQuantity(int id) {
        Product* p = searchById(id);
        if (p == nullptr) {
            return -1;
        }
        return p->quantity;
    }

    void DefaultProducts() {
        addProduct(1, "Sukkari dates", "Dates", 45.0, 20);
        addProduct(2, "Saudi Coffee", "Food", 30.0, 50);
        addProduct(3, "Kabsa Spices", "Spices", 15.0, 40);
        addProduct(4, "Local Ghee", "Food", 120.0, 5);
        addProduct(5, "Sidr Honey", "Food", 50.0, 15);
        addProduct(6, "Zamzam Water", "Drinks", 20.0, 100);
    }
};

struct OrderItem {
    int productId;
    int quantity;
    OrderItem* next;
};

struct Order {
    int orderId;
    string timestamp;
    OrderItem* items;
    Order* next;

    Order() {
        items = nullptr;
        next = nullptr;
    }
};

class OrderQueue {
private:
    Order* front;
    Order* rear;
    int orderCounter;

    void addItemToOrder(Order* order, int productId, int qty) {
        OrderItem* newItem = new OrderItem();
        newItem->productId = productId;
        newItem->quantity = qty;
        newItem->next = nullptr;

        if (order->items == nullptr) {
            order->items = newItem;
        } else {
            OrderItem* temp = order->items;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newItem;
        }
    }

    string getCurrentTime() {
        time_t now = time(0);
        char* dt = ctime(&now);
        string timeStr(dt);
        if (!timeStr.empty() && timeStr[timeStr.length() - 1] == '\n') {
            timeStr.erase(timeStr.length() - 1);
        }
        return timeStr;
    }

public:
    OrderQueue() {
        front = nullptr;
        rear = nullptr;
        orderCounter = 1000;
    }

    bool isEmpty() {
        return front == nullptr;
    }

    void enqueueOrder() {
        Order* newOrder = new Order();
        newOrder->orderId = orderCounter++;
        newOrder->timestamp = getCurrentTime();

        cout << "\nCreate New Order" << '\n';
        cout << "Order ID: " << newOrder->orderId << '\n';
        cout << "How many items to add? ";
        int numItems;
        cin >> numItems;

        for (int i = 0; i < numItems; i++) {
            int prodId, qty;
            cout << "Item " << (i + 1) << " - Enter Product ID: ";
            cin >> prodId;
            cout << "Item " << (i + 1) << " - Enter Quantity: ";
            cin >> qty;
            addItemToOrder(newOrder, prodId, qty);
        }

        if (rear == nullptr) {
            front = rear = newOrder;
        } else {
            rear->next = newOrder;
            rear = newOrder;
        }

        cout << "Order #" << newOrder->orderId << " added to queue." << '\n';
    }

    void dequeueAndProcess(ProductCatalog& catalog) {
        if (front == nullptr) {
            cout << "No orders in queue to process." << '\n';
            return;
        }

        Order* orderToProcess = front;
        cout << "\nProcessing order #" << orderToProcess->orderId << '\n';
        cout << "Time: " << orderToProcess->timestamp << '\n';
        cout << "------------------------------" << '\n';

        double totalAmount = 0.0;
        OrderItem* item = orderToProcess->items;

        while (item != nullptr) {
            Product* product = catalog.searchById(item->productId);

            if (product == nullptr) {
                cout << "Product ID " << item->productId
                     << " not found. Item skipped." << '\n';
            } else {
                int availableQty = catalog.getQuantity(item->productId);

                if (availableQty < item->quantity) {
                    cout << "Product " << product->name
                         << " (ID: " << item->productId << ") - Only "
                         << availableQty << " in stock, requested "
                         << item->quantity << ". Item skipped." << '\n';
                } else {
                    double itemTotal = product->price * item->quantity;
                    catalog.reduceQuantity(item->productId, item->quantity);

                    cout << product->name
                         << " x" << item->quantity
                         << " @ " << product->price << " = "
                         << itemTotal << " SAR" << '\n';

                    totalAmount += itemTotal;
                }
            }

            item = item->next;
        }

        cout << "------------------------------" << '\n';
        cout << "Total: " << totalAmount << " SAR" << '\n';

        front = front->next;
        if (front == nullptr) {
            rear = nullptr;
        }

        OrderItem* currentItem = orderToProcess->items;
        while (currentItem != nullptr) {
            OrderItem* temp = currentItem;
            currentItem = currentItem->next;
            delete temp;
        }
        delete orderToProcess;
    }

    void displayQueue() {
        if (front == nullptr) {
            cout << "Queue is empty." << '\n';
            return;
        }

        cout << "\nOrders in queue:" << '\n';
        Order* temp = front;
        int position = 1;

        while (temp != nullptr) {
            cout << position << ") Order ID: " << temp->orderId
                 << " | Time: " << temp->timestamp << '\n';
            cout << "   Items: ";
            OrderItem* item = temp->items;
            while (item != nullptr) {
                cout << "(Product:" << item->productId
                     << ", Qty:" << item->quantity << ") ";
                item = item->next;
            }
            cout << '\n';
            temp = temp->next;
            position++;
        }
    }
};

void showMenu() {
    cout << "\nStore System Menu" << '\n';
    cout << "1. Add new product" << '\n';
    cout << "2. Display catalog" << '\n';
    cout << "3. Update product" << '\n';
    cout << "4. Delete product" << '\n';
    cout << "5. Create new order" << '\n';
    cout << "6. Process next order" << '\n';
    cout << "7. Display orders in queue" << '\n';
    cout << "0. Exit" << '\n';
    cout << "Enter choice: ";
}

int main() {
    ProductCatalog catalog;
    catalog.DefaultProducts();

    OrderQueue orderQueue;
    int choice;

    while (true) {
        showMenu();
        cin >> choice;

        if (choice == 0) {
            cout << "Exiting program..." << '\n';
            break;
        }

        if (choice == 1) {
            int id, quantity;
            double price;
            string name, category;

            cout << "\nAdd New Product" << '\n';
            cout << "Enter product ID: ";
            cin >> id;
            cin.ignore();

            cout << "Enter product name: ";
            getline(cin, name);

            cout << "Enter category: ";
            getline(cin, category);

            cout << "Enter price: ";
            cin >> price;

            cout << "Enter quantity: ";
            cin >> quantity;

            catalog.addProduct(id, name, category, price, quantity);
            cout << "Product added." << '\n';
        }
        else if (choice == 2) {
            cout << "\nProduct Catalog" << '\n';
            catalog.display();
        }
        else if (choice == 3) {
            int id, newQuantity;
            double newPrice;

            cout << "\nUpdate Product" << '\n';
            cout << "Enter product ID: ";
            cin >> id;

            cout << "Enter new price: ";
            cin >> newPrice;

            cout << "Enter new quantity: ";
            cin >> newQuantity;

            catalog.updateProduct(id, newPrice, newQuantity);
        }
        else if (choice == 4) {
            int id;
            cout << "\nDelete Product" << '\n';
            cout << "Enter product ID: ";
            cin >> id;
            catalog.deleteProduct(id);
        }
        else if (choice == 5) {
            cout << "\nNew Order" << '\n';
            cout << "Current catalog:" << '\n';
            catalog.display();
            orderQueue.enqueueOrder();
        }
        else if (choice == 6) {
            cout << "\nProcess Next Order" << '\n';
            orderQueue.dequeueAndProcess(catalog);
        }
        else if (choice == 7) {
            orderQueue.displayQueue();
        }
        else {
            cout << "Invalid choice." << '\n';
        }
    }

    return 0;
}
