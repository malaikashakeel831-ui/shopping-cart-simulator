#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <iomanip>
using namespace std;

struct Product {
    int id;
    char name[50];
    float price;
    int stock;
};

struct cartitem {
    int id;
    char name[50];
    float price;
    int quantity;
    float discountApplied = 0; // store discount applied on item
};

struct Coupon {
    char code[20];
    char productName[50];
    float discount; // percentage
};

// Global arrays
Product* products = nullptr;
int productcount = 0;

cartitem* cart = nullptr;
int cartcount = 0;

Coupon* coupons = nullptr;
int couponCount = 0;

void loadproducts() {
    ifstream fin("products.txt");
    if (!fin) {
        cout << "products.txt not found!\n";
        return;
    }
    productcount = 0;
    Product temp[100];
    while (fin >> temp[productcount].id >> temp[productcount].name
               >> temp[productcount].price >> temp[productcount].stock) {
        productcount++;
    }
    fin.close();

    products = new Product[productcount];
    for (int i = 0; i < productcount; i++) {
        products[i] = temp[i];
    }
    cart = new cartitem[100];
    cartcount = 0;
}

void loadCoupons() {
    ifstream fin("coupons.txt");
    if (!fin) {
        cout << "coupons.txt not found!\n";
        return;
    }
    couponCount = 0;
    Coupon temp[100];
    while (fin >> temp[couponCount].code >> temp[couponCount].productName >> temp[couponCount].discount) {
        couponCount++;
    }
    fin.close();
    coupons = new Coupon[couponCount];
    for (int i = 0; i < couponCount; i++) coupons[i] = temp[i];
}

void displayproducts() {
    cout << "\nProducts:\n";
    if (productcount == 0) {
        cout << "No products loaded.\n";
        return;
    }
    for (int i = 0; i < productcount; i++) {
        cout << products[i].id << " | " << products[i].name
             << " | Price: " << products[i].price
             << " | Stock: " << products[i].stock << endl;
    }
}

void searchProducts(const string& keyword) {
    cout << "\nSearch Results for '" << keyword << "':\n";
    bool found = false;

    for (int i = 0; i < productcount; i++) {
        string pname(products[i].name);
        
        if (pname.find(keyword) != string::npos) {
            cout << products[i].id << " | " << products[i].name
                 << " | Price: " << products[i].price
                 << " | Stock: " << products[i].stock << endl;
            found = true;
        }
    }

    if (!found) {
        cout << "No matching products found.\n";
    }
}

void savecart() {
    ofstream fout("cart.txt");
    for (int i = 0; i < cartcount; i++) {
        fout << cart[i].id << " " << cart[i].name << " "
             << cart[i].price << " " << cart[i].quantity << " "
             << cart[i].price * cart[i].quantity << " "
             << cart[i].discountApplied << "%" << endl;
    }
    fout.close();
}

void addtocart(int productId, int qty) {
    for (int i = 0; i < productcount; i++) {
        if (products[i].id == productId) {
            if (products[i].stock < qty) {
                cout << "Not enough stock!\n";
                return;
            }

            // If product already in cart
            for (int j = 0; j < cartcount; j++) {
                if (cart[j].id == productId) {
                    cart[j].quantity += qty;
                    products[i].stock -= qty;
                    cout << "Updated " << cart[j].name << " quantity.\n";

                    // Coupon check
                    char choice;
                    cout << "Do you have a coupon code for this product? (y/n): ";
                    cin >> choice;
                    if (choice == 'y' || choice == 'Y') {
                        string code;
                        cout << "Enter coupon code: ";
                        cin >> code;
                        for (int c = 0; c < couponCount; c++) {
                            if (code == coupons[c].code &&
                                strcmp(cart[j].name, coupons[c].productName) == 0) {
                                cart[j].discountApplied = coupons[c].discount;
                                cout << "Coupon applied: " << coupons[c].discount
                                     << "% off on " << cart[j].name << endl;
                                break;
                            }
                        }
                    }

                    savecart();
                    return;
                }
            }

            // Add as new item
            cart[cartcount].id = products[i].id;
            strcpy(cart[cartcount].name, products[i].name);
            cart[cartcount].price = products[i].price;
            cart[cartcount].quantity = qty;
            cart[cartcount].discountApplied = 0;
            products[i].stock -= qty;
            cout << "Added " << products[i].name << " to cart.\n";

            // Coupon check
            char choice;
            cout << "Do you have a coupon code for this product? (y/n): ";
            cin >> choice;
            if (choice == 'y' || choice == 'Y') {
                string code;
                cout << "Enter coupon code: ";
                cin >> code;
                for (int c = 0; c < couponCount; c++) {
                    if (code == coupons[c].code &&
                        strcmp(cart[cartcount].name, coupons[c].productName) == 0) {
                        cart[cartcount].discountApplied = coupons[c].discount;
                        cout << "Coupon applied: " << coupons[c].discount
                             << "% off on " << cart[cartcount].name << endl;
                        break;
                    }
                }
            }

            cartcount++;
            savecart();
            return;
        }
    }
    cout << "Product not found.\n";
}

void removefromcart(int productId) {
    for (int i = 0; i < cartcount; i++) {
        if (cart[i].id == productId) {
            for (int j = 0; j < productcount; j++) {
                if (products[j].id == productId) {
                    products[j].stock += cart[i].quantity;
                }
            }
            for (int k = i; k < cartcount - 1; k++) cart[k] = cart[k + 1];
            cartcount--;
            cout << "Removed from cart.\n";
            savecart();
            return;
        }
    }
    cout << "Not in cart.\n";
}

void updatecartquantity(int productId, int newQty) {
    for (int i = 0; i < cartcount; i++) {
        if (cart[i].id == productId) {
            for (int j = 0; j < productcount; j++) {
                if (products[j].id == productId) {
                    products[j].stock += cart[i].quantity;
                    if (products[j].stock < newQty) {
                        cout << "Not enough stock.\n";
                        products[j].stock -= cart[i].quantity;
                        return;
                    }
                    cart[i].quantity = newQty;
                    products[j].stock -= newQty;
                    cout << "Quantity updated.\n";
                    savecart();
                    return;
                }
            }
        }
    }
    cout << "Item not found in cart.\n";
}

void viewCart() {
    cout << "\nYour Cart:\n";
    if (cartcount == 0) {
        cout << "Cart empty.\n";
        return;
    }
    float total = 0;
    for (int i = 0; i < cartcount; i++) {
        float sub = cart[i].price * cart[i].quantity;
        float discount = (cart[i].discountApplied / 100.0) * sub;
        total += sub - discount;

        cout << cart[i].id << " | " << cart[i].name
             << " | Price: " << cart[i].price
             << " | Qty: " << cart[i].quantity
             << " | Subtotal: " << sub;
        if (cart[i].discountApplied > 0)
            cout << " | Discount Applied: " << cart[i].discountApplied << "%";
        cout << " | Final: " << sub - discount << endl;
    }
    cout << "Total (after discounts, before tax): " << total << endl;
}

void checkout() {
    if (cartcount == 0) {
        cout << "Nothing to checkout.\n";
        return;
    }
    ofstream fout("receipt.txt");
    fout << "===== RECEIPT =====\n";
    
    float subtotal = 0, discountTotal = 0;
    for (int i = 0; i < cartcount; i++) {
        float sub = cart[i].price * cart[i].quantity;
        float discount = (cart[i].discountApplied / 100.0) * sub;
        discountTotal += discount;
        subtotal += sub;

        fout << cart[i].id << " | " << cart[i].name
             << " | Price: " << cart[i].price
             << " | Qty: " << cart[i].quantity
             << " | Subtotal: " << sub;
        if (cart[i].discountApplied > 0)
            fout << " | Discount: -" << discount 
                 << " (" << cart[i].discountApplied << "%)";
        fout << endl;
    }

    // Apply tax after discounts
    float discountedTotal = subtotal - discountTotal;
    float tax = 0.10f * discountedTotal;
    float finalTotal = discountedTotal + tax;

    fout << "-------------------\n";
    fout << "Subtotal: " << subtotal << endl;
    fout << "Discounts Applied: -" << discountTotal << endl;
    fout << "Amount after Discount: " << discountedTotal << endl;
    fout << "Tax (10%): " << tax << endl;
    fout << "FINAL TOTAL: " << fixed << setprecision(2) << finalTotal << endl;
    fout << "Thanks for shopping!\n";
    fout.close();

    cout << "Checkout complete. Receipt saved as receipt.txt\n";
    cartcount = 0;
    savecart();
}

void menu() {
    int ch;
    do {
        cout << "\n1. View Products\n2. Search Products\n3. Add to Cart\n4. Remove from Cart\n";
        cout << "5. Update Quantity\n6. View Cart\n7. Checkout\n8. Exit\n";
        cout << "Choice: ";
        cin >> ch;
        if (ch == 1) displayproducts();
        else if (ch == 2) {
            string key;
            cout << "Enter product name/initial: ";
            cin >> key;
            searchProducts(key);
        }
        else if (ch == 3) {
            int id, q;
            cout << "Product ID: "; cin >> id;
            cout << "Qty: "; cin >> q;
            addtocart(id, q);
        }
        else if (ch == 4) {
            int id;
            cout << "Remove ID: "; cin >> id;
            removefromcart(id);
        }
        else if (ch == 5) {
            int id, q;
            cout << "Product ID: "; cin >> id;
            cout << "New Qty: "; cin >> q;
            updatecartquantity(id, q);
        }
        else if (ch == 6) viewCart();
        else if (ch == 7) checkout();
    } while (ch != 8);
    cout << "Goodbye!\n";
}

int main() {
    loadproducts();
    loadCoupons();
    menu();

    delete[] products;
    delete[] cart;
    delete[] coupons;
    return 0;
}
