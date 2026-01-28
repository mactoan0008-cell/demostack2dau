#include "thuvienlogic.h"
#include <QDebug>
#include <QDate>
#include <algorithm>

// =================================================================
// ===           1. CÁC HÀM TIỆN ÍCH HỖ TRỢ CHUNG               ===
// =================================================================

// --- XỬ LÝ CHUỖI ---
string XoaKhoangTrangDauCuoi(string s){
    while (!s.empty() && isspace(s.front())) s.erase(s.begin());
    while (!s.empty() && isspace(s.back())) s.pop_back();
    return s;
}

string XoaKhoangTrangThua(string s){
    string res; bool space = false;
    for (char c : s){
        if (isspace(c)){
            if (!space){ res += ' '; space = true; }
        } else {
            res += c; space = false;
        }
    }
    return XoaKhoangTrangDauCuoi(res);
}

string ChuanHoaChuoi(string s) {
    s = XoaKhoangTrangThua(s);
    for (size_t i = 0; i < s.length(); i++) s[i] = tolower(s[i]);
    if (!s.empty()) s[0] = toupper(s[0]);
    for (size_t i = 1; i < s.length(); i++) {
        if (isspace(s[i-1]) && isalpha(s[i])) s[i] = toupper(s[i]);
    }
    return s;
}

string ToLower(string s) {
    for (char &c : s) c = tolower(c);
    return s;
}

// Tìm kiếm chuỗi con (Dùng cho tìm kiếm sách theo tên)
bool Timkiem_ChuoiCon(const string& Chuoi_Chinh, const string& Chuoi_Con){
    if (Chuoi_Con.empty()) return true;
    if (Chuoi_Chinh.length() < Chuoi_Con.length()) return false;
    int n = Chuoi_Chinh.length(); int m = Chuoi_Con.length();
    for (int i = 0; i <= n - m; ++i){
        bool khop = true;
        for (int j = 0; j < m; ++j){
            if (Chuoi_Chinh[i + j] != Chuoi_Con[j]){
                khop = false; break;
            }
        }
        if (khop) return true;
    }
    return false;
}

// --- XỬ LÝ NGÀY THÁNG (Cần cho Module Thống Kê Quá Hạn) ---
Date LayNgayHienTai() {
    time_t t = time(0);
    tm* now = localtime(&t);
    Date d;
    d.d = now->tm_mday;
    d.m = now->tm_mon + 1;
    d.y = now->tm_year + 1900;
    return d;
}

bool La_Nam_Nhuan(int nam){
    return (nam % 4 == 0 && nam % 100 != 0) || (nam % 400 == 0);
}

int So_Ngay_Trong_Thang(int m, int y){
    switch(m){
    case 1: case 3: case 5: case 7: case 8: case 10: case 12: return 31;
    case 4: case 6: case 9: case 11: return 30;
    case 2: return La_Nam_Nhuan(y) ? 29 : 28;
    }
    return 0;
}

void Doi_Ngay_Thang_Nam(Date &date){
    int n = So_Ngay_Trong_Thang(date.m, date.y);
    while(date.d > n){
        date.d -= n;
        date.m += 1;
        if(date.m > 12){ date.m = 1; date.y += 1; }
        n = So_Ngay_Trong_Thang(date.m, date.y);
    }
}

// Kiểm tra xem ngày mượn đã quá hạn chưa (So với ngày hiện tại)
bool KTQuaHan(Date &ngayMuon){
    Date today = LayNgayHienTai();
    Date hanTra = ngayMuon;
    hanTra.d += 7; // Cộng 7 ngày hạn
    Doi_Ngay_Thang_Nam(hanTra);

    if(today.y > hanTra.y) return true;
    if(today.y == hanTra.y && today.m > hanTra.m) return true;
    if(today.y == hanTra.y && today.m == hanTra.m && today.d > hanTra.d) return true;
    return false;
}

// Tính số ngày chênh lệch (Dùng để tính số ngày quá hạn)
int Tinh_Khoang_Cach_Ngay(Date ngayTruoc, Date ngaySau) {
    QDate qTruoc(ngayTruoc.y, ngayTruoc.m, ngayTruoc.d);
    QDate qSau(ngaySau.y, ngaySau.m, ngaySau.d);
    return qTruoc.daysTo(qSau);
}

void Swap(nodeDG* &a, nodeDG* &b){
    nodeDG* tmp=a; a=b; b=tmp;
}

// =================================================================
// ===           2. KHỞI TẠO & QUẢN LÝ DỮ LIỆU                  ===
// =================================================================

ThuVienLogic::ThuVienLogic(QObject *parent) : QObject{parent}
{
    this->dsDocGia = NULL;
    this->dsds.n = 0;
}

ThuVienLogic::~ThuVienLogic()
{
    // Chỉ giải phóng vùng nhớ Sách
    GiaiPhongDanhSachDauSach();
    // (Phần giải phóng cây độc giả đã lược bỏ)
}

void ThuVienLogic::KhoiTaoVaTaiDuLieu()
{
    // Chỉ tải dữ liệu Sách (Module Độc Giả bị lược bỏ phần CRUD nhưng vẫn cần load để thống kê)
    // this->TaiFileDocGia(); // (Giữ lại nếu muốn chạy Thống kê thực tế)
    this->TaiFileDauSach();
    qDebug() << "Da tai xong du lieu Sach.";
}

void ThuVienLogic::LuuDuLieuKhiThoat()
{
    this->LuuFileDauSach();
}

// =================================================================
// ===           3. MODULE QUẢN LÝ SÁCH                         ===
// =================================================================

// --- Các hàm tìm kiếm & Tiện ích nội bộ ---

DauSach* ThuVienLogic::TimDauSachTheoISBN(string isbn) {
    for (int i = 0; i < this->dsds.n; i++) {
        if (this->dsds.nodes[i]->ISBN == isbn) return this->dsds.nodes[i];
    }
    return NULL;
}

// Logic: Ưu tiên Thể loại -> Tên sách
bool ThuVienLogic::SoSanhTheoTheLoaiVaTen(DauSach* a, DauSach* b) {
    if (a->theloai < b->theloai) return true;
    if (a->theloai == b->theloai && a->tensach < b->tensach) return true;
    return false;
}

// Tìm số đuôi lớn nhất để sinh mã tự động (VD: ISBN-005 -> trả về 5)
int ThuVienLogic::LaySoThuTuLonNhat(DauSach* dauSach) {
    int soLonNhat = 0;
    for (nodeSach* p = dauSach->firstDMS; p != NULL; p = p->next) {
        string maDayDu = p->sach.MASACH;
        size_t viTriGachCuoi = maDayDu.find_last_of('-');
        if (viTriGachCuoi != string::npos) {
            try {
                string duoiSo = maDayDu.substr(viTriGachCuoi + 1);
                int soThuTu = stoi(duoiSo);
                if (soThuTu > soLonNhat) soLonNhat = soThuTu;
            } catch (...) { continue; }
        }
    }
    return soLonNhat;
}

string ThuVienLogic::LayViTriTheoTheLoai(string theLoai) {
    string tl = ChuanHoaChuoi(theLoai);
    if (tl == "Tin Học" || tl == "Công Nghệ Thông Tin") return "Kệ A - Tầng 1 (Khu IT)";
    if (tl == "Văn Học" || tl == "Tiểu Thuyết") return "Kệ B - Tầng 1 (Khu Văn Học)";
    // ... (Các kệ khác)
    return "Kệ Kho Tổng Hợp";
}

int ThuVienLogic::DemSoSachCon(DauSach* ds) {
    if (ds == NULL) return 0;
    int dem = 0;
    for (nodeSach* p = ds->firstDMS; p != NULL; p = p->next) dem++;
    return dem;
}

// --- CÁC HÀM NGHIỆP VỤ CHÍNH (THÊM, XÓA, SỬA ĐẦU SÁCH) ---

bool ThuVienLogic::ThemDauSach(string isbn, string tenSach, string tacGia, string theLoai, int soTrang, int namXB) {
    if (dsds.n >= MAX_DAUSACH) {
        emit guiThongBao("Lỗi: Bộ nhớ danh sách đầy!");
        return false;
    }
    if (TimDauSachTheoISBN(isbn) != NULL) {
        emit guiThongBao("Lỗi: ISBN này đã tồn tại!");
        return false;
    }

    // 1. Khởi tạo
    DauSach* pMoi = new DauSach;
    pMoi->ISBN = isbn;
    pMoi->tensach = ChuanHoaChuoi(tenSach);
    pMoi->tacgia = ChuanHoaChuoi(tacGia);
    pMoi->theloai = ChuanHoaChuoi(theLoai);
    pMoi->sotrang = soTrang;
    pMoi->namxb = namXB;
    pMoi->firstDMS = NULL;
    pMoi->luotmuon = 0;

    // 2. Chèn có sắp xếp (Insertion Sort)
    int pos = 0;
    while (pos < dsds.n && dsds.nodes[pos]->tensach < pMoi->tensach) {
        pos++;
    }
    for (int i = dsds.n; i > pos; i--) {
        dsds.nodes[i] = dsds.nodes[i - 1];
    }
    dsds.nodes[pos] = pMoi;
    dsds.n++;

    emit duLieuDauSachThayDoi();
    emit guiThongBao("Thêm đầu sách mới thành công!");
    return true;
}

bool ThuVienLogic::HieuChinhDauSach(string isbn, string tenSach, string tacGia, string theLoai, int soTrang, int namXB)
{
    DauSach* dauSachCanSua = TimDauSachTheoISBN(isbn);
    if (dauSachCanSua == NULL) {
        emit guiThongBao("Lỗi: Không tìm thấy ISBN để sửa!");
        return false;
    }
    dauSachCanSua->tensach = ChuanHoaChuoi(tenSach);
    dauSachCanSua->tacgia = ChuanHoaChuoi(tacGia);
    dauSachCanSua->theloai = ChuanHoaChuoi(theLoai);
    dauSachCanSua->sotrang = soTrang;
    dauSachCanSua->namxb = namXB;

    // Sắp xếp lại sau khi sửa tên (Bubble Sort)
    for (int i = 0; i < this->dsds.n - 1; ++i) {
        for (int j = 0; j < this->dsds.n - i - 1; ++j) {
            if (this->dsds.nodes[j+1]->tensach < this->dsds.nodes[j]->tensach) {
                swap(this->dsds.nodes[j], this->dsds.nodes[j+1]);
            }
        }
    }
    emit duLieuDauSachThayDoi();
    emit guiThongBao("Đã cập nhật đầu sách thành công!");
    return true;
}

int ThuVienLogic::XoaDauSach(string isbn) {
    int pos = -1;
    for (int i = 0; i < dsds.n; i++) {
        if (dsds.nodes[i]->ISBN == isbn) { pos = i; break; }
    }
    if (pos == -1) return 0;

    // Kiểm tra sách đang mượn
    PTRDMS pCheck = dsds.nodes[pos]->firstDMS;
    while (pCheck != NULL) {
        if (pCheck->sach.trangthai == SACH_DANG_MUON) return -1;
        pCheck = pCheck->next;
    }

    // Xóa sách con
    PTRDMS k = dsds.nodes[pos]->firstDMS;
    while (k != NULL) {
        PTRDMS temp = k;
        k = k->next;
        delete temp;
    }

    // Xóa đầu sách và dời mảng
    delete dsds.nodes[pos];
    for (int i = pos; i < dsds.n - 1; i++) {
        dsds.nodes[i] = dsds.nodes[i + 1];
    }
    dsds.n--;

    emit duLieuDauSachThayDoi();
    return 1;
}

// --- QUẢN LÝ SÁCH CON (KHO) ---

void ThuVienLogic::ThemSachCon(string isbn, int soLuongCanThem, string& dsMaSachTao) {
    DauSach* dauSach = TimDauSachTheoISBN(isbn);
    if (dauSach == NULL) return;

    string viTriKeSach = LayViTriTheoTheLoai(dauSach->theloai);
    int soThuTuHienTai = LaySoThuTuLonNhat(dauSach);
    
    // Tìm đuôi
    nodeSach* tail = dauSach->firstDMS;
    if (tail != NULL) {
        while (tail->next != NULL) tail = tail->next;
    }

    for (int i = 0; i < soLuongCanThem; i++) {
        soThuTuHienTai++;
        nodeSach* p = new nodeSach;

        // Sinh mã: ISBN + "-" + 000X
        string duoiMaSach = to_string(soThuTuHienTai);
        while (duoiMaSach.length() < 4) duoiMaSach = "0" + duoiMaSach;

        p->sach.MASACH = isbn + "-" + duoiMaSach;
        p->sach.trangthai = SACH_CHO_MUON;
        p->sach.vitri = viTriKeSach;
        p->next = NULL;

        if (dauSach->firstDMS == NULL) {
            dauSach->firstDMS = p;
            tail = p;
        } else {
            tail->next = p;
            tail = p;
        }
        dsMaSachTao += p->sach.MASACH + " ";
    }
    emit duLieuDauSachThayDoi();
}

int ThuVienLogic::XoaBotSachTrongKho(string isbn, int soLuongCanXoa) {
    DauSach* ds = TimDauSachTheoISBN(isbn);
    if (ds == NULL || ds->firstDMS == NULL) return 0;
    int daXoa = 0;

    while (daXoa < soLuongCanXoa) {
        nodeSach* p = ds->firstDMS;
        nodeSach* prev = NULL;
        bool timThayDeXoa = false;

        while (p != NULL) {
            // Chỉ xóa sách trong kho hoặc đã thanh lý
            if (p->sach.trangthai == SACH_CHO_MUON || p->sach.trangthai == SACH_THANH_LY) {
                if (p == ds->firstDMS) ds->firstDMS = p->next;
                else prev->next = p->next;

                delete p;
                timThayDeXoa = true;
                daXoa++;
                break;
            }
            prev = p;
            p = p->next;
        }
        if (!timThayDeXoa) break;
    }
    return daXoa;
}

string ThuVienLogic::ChuyenTrangThaiThanhChu(int trangthai) {
    if (trangthai == SACH_CHO_MUON) return "Cho mượn được";
    if (trangthai == SACH_DANG_MUON) return "Đang mượn";
    if (trangthai == SACH_THANH_LY) return "Đã thanh lý";
    return "Lỗi";
}

bool ThuVienLogic::ThanhLySach(string isbn, string maSach) {
    DauSach* ds = TimDauSachTheoISBN(isbn);
    if (ds == NULL) return false;
    nodeSach* p = ds->firstDMS;
    while (p != NULL) {
        if (p->sach.MASACH == maSach) {
            if (p->sach.trangthai == SACH_CHO_MUON) {
                p->sach.trangthai = SACH_THANH_LY;
                return true;
            }
            return false;
        }
        p = p->next;
    }
    return false;
}

bool ThuVienLogic::CapNhatViTriSach(string isbn, string maSach, string vitriMoi) {
    DauSach* ds = TimDauSachTheoISBN(isbn);
    if (ds == NULL) return false;
    nodeSach* p = ds->firstDMS;
    while (p != NULL) {
        if (p->sach.MASACH == maSach) {
            p->sach.vitri = vitriMoi;
            return true;
        }
        p = p->next;
    }
    return false;
}

// --- TÌM KIẾM & HIỂN THỊ ---

void ThuVienLogic::LayDanhSachDauSach(DauSach* mangDauSach[], int &soLuong, bool sapXepTheoTheLoai) {
    soLuong = 0;
    for (int i = 0; i < this->dsds.n; ++i) {
        mangDauSach[i] = this->dsds.nodes[i];
    }
    soLuong = this->dsds.n;
    if(soLuong == 0 || !sapXepTheoTheLoai) return;

    // Bubble Sort: Thể loại -> Tên
    for (int i = 0; i < soLuong - 1; ++i) {
        for (int j = 0; j < soLuong - i - 1; ++j) {
            if (SoSanhTheoTheLoaiVaTen(mangDauSach[j + 1], mangDauSach[j])) {
                swap(mangDauSach[j], mangDauSach[j+1]);
            }
        }
    }
}

void ThuVienLogic::TimSachTheoTen(string tuKhoa, DauSach* ketQua[], int &soLuongKQ) {
    soLuongKQ = 0;
    tuKhoa = ToLower(ChuanHoaChuoi(tuKhoa));
    if (tuKhoa.empty()) return;
    for (int i = 0; i < this->dsds.n; i++) {
        string tenSach = ToLower(this->dsds.nodes[i]->tensach);
        if (Timkiem_ChuoiCon(tenSach, tuKhoa)){
            if(soLuongKQ < MAX_DAUSACH) ketQua[soLuongKQ++] = this->dsds.nodes[i];
        }
    }
}

// --- FILE I/O (SÁCH) ---

void ThuVienLogic::GhiDauSachVaoFile(ofstream &fout, DauSach* ds) {
    if (ds == NULL) return;
    fout << ds->ISBN << "|" << ds->tensach << "|" << ds->sotrang << "|" << ds->tacgia << "|" << ds->namxb << "|" << ds->theloai << "|" << ds->luotmuon << endl;
    PTRDMS p = ds->firstDMS;
    while (p != NULL) {
        fout << p->sach.MASACH << "|" << p->sach.trangthai << "|" << p->sach.vitri << endl;
        p = p->next;
    }
    fout << endl;
}

void ThuVienLogic::LuuFileDauSach() {
    ofstream fout("DauSach.txt");
    if (!fout) { emit guiThongBao("Lỗi: Không thể mở file DauSach.txt"); return; }
    for (int i = 0; i < this-> dsds.n; i++) {
        GhiDauSachVaoFile(fout, dsds.nodes[i]);
    }
    fout.close();
}

void ThuVienLogic::TaiFileDauSach() {
    ifstream fin("DauSach.txt");
    if(!fin) return;
    string line;
    DauSach* currentDS = nullptr;
    nodeSach* currentTail = nullptr;

    while (getline(fin, line)) {
        if (line.empty()) { currentDS = nullptr; continue; }
        int soDauPhay = 0;
        for (char c : line) if (c == '|') soDauPhay++;
        stringstream ss(line);

        if (soDauPhay == 6) { // Load đầu sách
            currentDS = new DauSach; string token;
            getline(ss, currentDS->ISBN, '|');
            getline(ss, currentDS->tensach, '|');
            getline(ss, token, '|'); currentDS->sotrang = stoi(token);
            getline(ss, currentDS->tacgia, '|');
            getline(ss, token, '|'); currentDS->namxb = stoi(token);
            getline(ss, currentDS->theloai, '|');
            getline(ss, token, '|'); currentDS->luotmuon = stoi(token);

            currentDS->tensach = ChuanHoaChuoi(currentDS->tensach);
            currentDS->firstDMS = NULL;
            dsds.nodes[dsds.n++] = currentDS;
            currentTail = nullptr;
        }
        else if (soDauPhay == 2 && currentDS != nullptr) { // Load sách con
            nodeSach* newSach = new nodeSach; string token;
            getline(ss, newSach->sach.MASACH, '|');
            getline(ss, token, '|'); newSach->sach.trangthai = stoi(token);
            getline(ss, newSach->sach.vitri, '|');
            newSach->next = NULL;

            if (currentDS->firstDMS == NULL) {
                currentDS->firstDMS = newSach;
                currentTail = newSach;
            } else {
                currentTail->next = newSach;
                currentTail = newSach;
            }
        }
    }
    fin.close();
}

void ThuVienLogic::GiaiPhongDanhSachDauSach() {
    for (int i = 0; i < dsds.n; i++) {
        if (dsds.nodes[i] != nullptr) {
            PTRDMS p = dsds.nodes[i]->firstDMS;
            while (p != nullptr) {
                PTRDMS temp = p; p = p->next; delete temp;
            }
            delete dsds.nodes[i];
            dsds.nodes[i] = nullptr;
        }
    }
    dsds.n = 0;
}

// =================================================================
// ===           4. MODULE THỐNG KÊ (LOGIC)                     ===
// =================================================================

// Hàm đệ quy duyệt cây độc giả để tìm sách quá hạn
// (Cần cấu trúc cây độc giả nhưng không cần chức năng Thêm/Xóa độc giả)
void Duyet_DG_Muon_QuaHan(treeDG root, DocGiaQuaHan mangKetQua[], int& soLuong, Date mocThoiGian) {
    if (root == NULL) return;
    Duyet_DG_Muon_QuaHan(root->left, mangKetQua, soLuong, mocThoiGian);

    int maxQuaHan = 0;
    bool coViPham = false;

    for (PTRMT mt = root->dg.firstMT; mt != NULL; mt = mt->next){
        Date hanTra = mt->mt.NgayMuon;
        hanTra.d += 7;
        Doi_Ngay_Thang_Nam(hanTra);

        int soNgayTre = 0;
        if (mt->mt.trangthai == MT_DANG_MUON) {
            soNgayTre = Tinh_Khoang_Cach_Ngay(hanTra, mocThoiGian);
        } else if (mt->mt.trangthai == MT_DA_TRA) {
            if (Tinh_Khoang_Cach_Ngay(mt->mt.NgayTra, mocThoiGian) >= 0) {
                 soNgayTre = Tinh_Khoang_Cach_Ngay(hanTra, mt->mt.NgayTra);
            }
        }

        if (soNgayTre > 0) {
            coViPham = true;
            if (soNgayTre > maxQuaHan) maxQuaHan = soNgayTre;
        }
    }

    if (coViPham) {
        mangKetQua[soLuong].dg = &(root->dg);
        mangKetQua[soLuong].soNgayMax = maxQuaHan;
        soLuong++;
    }

    Duyet_DG_Muon_QuaHan(root->right, mangKetQua, soLuong, mocThoiGian);
}

void ThuVienLogic::LayDanhSachQuaHan(DocGiaQuaHan mangKetQua[], int &soLuong, Date mocThoiGian) {
    soLuong = 0;
    Duyet_DG_Muon_QuaHan(this->dsDocGia, mangKetQua, soLuong, mocThoiGian);

    // Sắp xếp giảm dần theo số ngày quá hạn
    for (int i = 0; i < soLuong - 1; i++) {
        for (int j = 0; j < soLuong - i - 1; j++) {
            if (mangKetQua[j].soNgayMax < mangKetQua[j+1].soNgayMax) {
                swap(mangKetQua[j], mangKetQua[j+1]);
            }
        }
    }
}

void ThuVienLogic::LayTop10SachMuonNhieuNhat(DauSach* mangKetQua[], int &soLuong) {
    soLuong = 0;
    if (dsds.n == 0) return;

    DauSach* mangTam[MAX_DAUSACH];
    for (int i = 0; i < dsds.n; i++) mangTam[i] = dsds.nodes[i];

    // Sắp xếp: Lượt mượn giảm dần -> Tên tăng dần
    for (int i = 0; i < dsds.n - 1; i++){
        for (int j = 0; j < dsds.n - i - 1; j++){
            DauSach* a = mangTam[j];
            DauSach* b = mangTam[j + 1];

            if (a->luotmuon < b->luotmuon || (a->luotmuon == b->luotmuon && a->tensach > b->tensach)) {
                swap(mangTam[j], mangTam[j+1]);
            }
        }
    }
    // Lấy top 10
    int soIn = (dsds.n < 10) ? dsds.n : 10;
    for (int i = 0; i < soIn; i++){
        mangKetQua[soLuong++] = mangTam[i];
    }
}

// --- HỖ TRỢ HIỂN THỊ CHI TIẾT SÁCH (Tên người đang mượn) ---

treeDG ThuVienLogic::DuyetTimNguoiMuon(treeDG root, string maSach) {
    if (root == NULL) return NULL;
    nodeMT* p = root->dg.firstMT;
    while (p != NULL) {
        if (p->mt.MASACH == maSach && p->mt.trangthai == MT_DANG_MUON) return root;
        p = p->next;
    }
    treeDG kqTrai = DuyetTimNguoiMuon(root->left, maSach);
    if (kqTrai != NULL) return kqTrai;
    return DuyetTimNguoiMuon(root->right, maSach);
}

treeDG ThuVienLogic::TimDocGiaDangMuonSach(string maSach) {
    return DuyetTimNguoiMuon(this->dsDocGia, maSach);
}
