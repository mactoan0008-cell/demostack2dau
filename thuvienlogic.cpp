
// =================================================================
// ===                  MODULE: ĐẦU SÁCH                         ===
// =================================================================

// =================================================================
// 1. CÁC HÀM HỖ TRỢ (HELPER FUNCTIONS)
// =================================================================

// Tìm con trỏ Đầu Sách dựa vào mã ISBN.
// Dùng con trỏ để khi tìm thấy trả về địa chỉ gốc của cuốn sách trong bộ nhớ.
DauSach* ThuVienLogic::TimDauSachTheoISBN(string isbn) {
    for (int i = 0; i < this->dsds.n; i++) {
        if (this->dsds.nodes[i]->ISBN == isbn) return this->dsds.nodes[i];
    }
    return NULL;
}

// Logic so sánh để sắp xếp: Ưu tiên Thể loại -> Sau đó đến Tên sách
bool ThuVienLogic::SoSanhTheoTheLoaiVaTen(DauSach* a, DauSach* b) {
    if (a->theloai < b->theloai) return true;
    if (a->theloai == b->theloai && a->tensach < b->tensach) return true;
    return false;
}

int ThuVienLogic::LaySoThuTuLonNhat(DauSach* dauSach) {
    int soLonNhat = 0;
    //Vòng lặp duyệt dslk
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

// Quy định vị trí kệ sách dựa trên tên Thể loại
string ThuVienLogic::LayViTriTheoTheLoai(string theLoai) {
    string tl = ChuanHoaChuoi(theLoai);
    if (tl == "Tin Học" || tl == "Công Nghệ Thông Tin" || tl == "Lập Trình") return "Kệ A - Tầng 1 (Khu IT)";
    if (tl == "Văn Học" || tl == "Tiểu Thuyết" || tl == "Truyện Ngắn") return "Kệ B - Tầng 1 (Khu Văn Học)";
    if (tl == "Toán Học" || tl == "Khoa Học Tự Nhiên") return "Kệ C - Tầng 2 (Khu KHTN)";
    if (tl == "Ngoại Ngữ" || tl == "Tiếng Anh") return "Kệ D - Tầng 2 (Khu Ngoại Ngữ)";
    if (tl == "Truyện Tranh" || tl == "Manga") return "Kệ E - Tầng 1 (Khu Giải Trí)";
    if (tl == "Kinh Tế" || tl == "Tài Chính") return "Kệ F - Tầng 2 (Khu Kinh Tế)";
    if (tl == "Tâm Lí Học" || tl == "Kỹ Năng Sống" || tl == "Tâm Lý") return "Kệ G - Tầng 1 (Khu Kỹ Năng)";
    return "Kệ Kho Tổng Hợp";
}

int ThuVienLogic::DemSoSachCon(DauSach* ds) {
    if (ds == NULL) return 0;
    int dem = 0;
    for (nodeSach* p = ds->firstDMS; p != NULL; p = p->next) {
        dem++;
    }
    return dem;
}

// =================================================================
// 2. CHỨC NĂNG CHÍNH: QUẢN LÝ ĐẦU SÁCH (THÊM, SỬA)
// =================================================================

// Thêm đầu sách mới (Có sắp xếp chèn ngay khi thêm)
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
    pMoi->firstDMS = NULL;// chưa có sách con 
    pMoi->luotmuon = 0;

    // 2. Tìm vị trí chèn (Insertion Sort)-câu d 
    int pos = 0;
    while (pos < dsds.n && dsds.nodes[pos]->tensach < pMoi->tensach) {
        pos++;
    }

    // 3. Dời mảng và chèn
    for (int i = dsds.n; i > pos; i--) {
        dsds.nodes[i] = dsds.nodes[i - 1];
    }
    dsds.nodes[pos] = pMoi;
    dsds.n++;

    emit duLieuDauSachThayDoi();
    emit guiThongBao("Thêm đầu sách mới thành công!");
    return true;
}

// Hiệu chỉnh thông tin đầu sách
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

    // Sau khi sửa tên, cần sắp xếp lại mảng (Bubble Sort đơn giản vì mảng đã gần như có thứ tự)
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
// Hàm trả về: 1 (Thành công), 0 (Không tìm thấy), -1 (Đang có người mượn)
int ThuVienLogic::XoaDauSach(string isbn) {
    // 1. Tìm vị trí đầu sách
    int pos = -1;
    for (int i = 0; i < dsds.n; i++) {
        if (dsds.nodes[i]->ISBN == isbn) {
            pos = i;
            break;
        }
    }

    if (pos == -1) return 0; // Không tìm thấy

    // 2. Kiểm tra an toàn: Có ai đang mượn không?
    PTRDMS pCheck = dsds.nodes[pos]->firstDMS;
    while (pCheck != NULL) {
        if (pCheck->sach.trangthai == SACH_DANG_MUON) {
            return -1; // Đang có người mượn -> Cấm xóa
        }
        pCheck = pCheck->next;
    }

    // 3. Dọn dẹp: Xóa sạch các sách con (Dùng PTRDMS)
    PTRDMS k = dsds.nodes[pos]->firstDMS;
    while (k != NULL) {
        PTRDMS temp = k;
        k = k->next;
        delete temp; // Giải phóng bộ nhớ
    }

    // 4. Xóa đầu sách
    delete dsds.nodes[pos];

    // 5. Dời mảng
    for (int i = pos; i < dsds.n - 1; i++) {
        dsds.nodes[i] = dsds.nodes[i + 1];
    }
    dsds.n--;

    // 6. Cập nhật giao diện
    emit duLieuDauSachThayDoi();
    return 1;
}
// =================================================================
// 3. CHỨC NĂNG PHỤ: QUẢN LÝ SÁCH CON (DMS)
// =================================================================

void ThuVienLogic::ThemSachCon(string isbn, int soLuongCanThem, string& dsMaSachTao) {
    DauSach* dauSach = TimDauSachTheoISBN(isbn);
    if (dauSach == NULL) return;

    string viTriKeSach = LayViTriTheoTheLoai(dauSach->theloai);
    int soThuTuHienTai = LaySoThuTuLonNhat(dauSach);
    // Dùng tail tìm xem cuốn sách nào đang nằm cuối(tail là cái đuôi)
    // Ví dụ: tail đang ở vị trí A kiểm tra "tail-> next" là (B) ,
    //        tail đang ở vị trí B kiểm tra "tail ->next" là NULL => B là đang là cuốn sách cuối cùng.
    nodeSach* tail = dauSach->firstDMS;
    if (tail != NULL) {
        while (tail->next != NULL) {
            tail = tail->next;
        }
    }

    for (int i = 0; i < soLuongCanThem; i++) {
        soThuTuHienTai++;

        // 1. Tạo node mới
        nodeSach* p = new nodeSach;

        // 2. Sinh mã sách: ISBN + "-" + 000X-câu 
        string duoiMaSach = to_string(soThuTuHienTai);
        while (duoiMaSach.length() < 4) duoiMaSach = "0" + duoiMaSach;

        // 3. Gán dữ liệu
        p->sach.MASACH = isbn + "-" + duoiMaSach;
        p->sach.trangthai = SACH_CHO_MUON;
        p->sach.vitri = viTriKeSach;
        p->next = NULL;// vị trí cuối cùng.

        // 4. Nối vào danh sách
        // Nếu chưa có gì: Cái mới vừa là Đầu vừa là Đuôi.
        if (dauSach->firstDMS == NULL) {
            dauSach->firstDMS = p;
            tail = p;
            // Nếu có rồi: Đuôi cũ móc vào cái Mới -> Cái Mới trở thành Đuôi.
        } else {
            tail->next = p;
            tail = p;
        }

        dsMaSachTao += p->sach.MASACH + " ";
    }
    emit duLieuDauSachThayDoi();
}
int ThuVienLogic::XoaBotSachTrongKho(string isbn, int soLuongCanXoa) {
    // 1. Tìm đầu sách
    DauSach* ds = TimDauSachTheoISBN(isbn);
    if (ds == NULL || ds->firstDMS == NULL) return 0;

    int daXoa = 0;

    // 2. Vòng lặp: Xóa từng cuốn một cho đến khi đủ số lượng
    while (daXoa < soLuongCanXoa) {
        nodeSach* p = ds->firstDMS;
        nodeSach* prev = NULL;
        bool timThayDeXoa = false;

        while (p != NULL) {
            // ĐIỀU KIỆN XÓA: Sách trong kho (0) hoặc Đã thanh lý (2)
            // TUYỆT ĐỐI KHÔNG XÓA SÁCH ĐANG MƯỢN (1)
            if (p->sach.trangthai == 0 || p->sach.trangthai == 2) {

                // Thực hiện xóa node khỏi DSLK đơn
                if (p == ds->firstDMS) {
                    ds->firstDMS = p->next; // Xóa đầu
                } else {
                    prev->next = p->next;   // Xóa giữa/cuối
                }

                delete p;

                timThayDeXoa = true;
                daXoa++;
                break;
            }

            // Đang được độc giả mượn sách => tiến tới cuốn khác
            prev = p;
            p = p->next;
        }

        // Nếu duyệt cả danh sách mà không tìm thấy cuốn nào xóa được
        if (!timThayDeXoa) {
            break;
        }
    }

    return daXoa;
}
// Hàm trả về chuỗi mô tả trạng thái để hiển thị
string ThuVienLogic::ChuyenTrangThaiThanhChu(int trangthai) {
    if (trangthai == SACH_CHO_MUON) return "Cho mượn được";
    if (trangthai == SACH_DANG_MUON) return "Đang mượn";
    if (trangthai == SACH_THANH_LY) return "Đã thanh lý";
    return "Lỗi/Không rõ";
}

int ThuVienLogic::LayTrangThaiSach(string isbn, string maSach) {
    DauSach* ds = TimDauSachTheoISBN(isbn);
    if (ds == NULL) return -1;

    for (nodeSach* p = ds->firstDMS; p != NULL; p = p->next) {
        if (p->sach.MASACH == maSach) {
            return p->sach.trangthai;
        }
    }
    return -1;
}
// Cập nhật trạng thái sách thành thanh lý
bool ThuVienLogic::ThanhLySach(string isbn, string maSach) {
    DauSach* ds = TimDauSachTheoISBN(isbn);
    if (ds == NULL) return false;

    nodeSach* p = ds->firstDMS;
    while (p != NULL) {
        if (p->sach.MASACH == maSach) {
            if (p->sach.trangthai == SACH_CHO_MUON) {
                p->sach.trangthai = SACH_THANH_LY;
                return true;
            } else {
                return false; // Đang mượn hoặc đã thanh lý rồi
            }
        }
        p = p->next;
    }
    return false;
}

// Cập nhật vị trí kệ sách cho một cuốn cụ thể
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

// =================================================================
// 4. CHỨC NĂNG TÌM KIẾM VÀ HIỂN THỊ
// =================================================================

// Lấy danh sách đầu sách (có hỗ trợ sắp xếp theo thể loại)-câu d 
void ThuVienLogic::LayDanhSachDauSach(DauSach* mangDauSach[], int &soLuong, bool sapXepTheoTheLoai) {
    soLuong = 0;
    for (int i = 0; i < this->dsds.n; ++i) {
        mangDauSach[i] = this->dsds.nodes[i];
    }
    soLuong = this->dsds.n;
    // Nếu tham số sapXepTheoTheLoai là TRUE thì mới bắt đầu sắp xếp
    if(soLuong == 0 || !sapXepTheoTheLoai) return;

    // Bubble Sort để sắp xếp theo Thể loại -> Tên
    for (int i = 0; i < soLuong - 1; ++i) {
        for (int j = 0; j < soLuong - i - 1; ++j) {
            if (SoSanhTheoTheLoaiVaTen(mangDauSach[j + 1], mangDauSach[j])) {
                swap(mangDauSach[j], mangDauSach[j+1]);
            }
        }
    }
}

// Tìm kiếm tương đối theo tên sách
void ThuVienLogic::TimSachTheoTen(string tuKhoa, DauSach* ketQua[], int &soLuongKQ) {
    soLuongKQ = 0;
    tuKhoa = ToLower(ChuanHoaChuoi(tuKhoa));
    if (tuKhoa.empty()) return;

    for (int i = 0; i < this->dsds.n; i++) {
        DauSach* ds = this->dsds.nodes[i];
        string tenSach = ToLower(ds->tensach);

        if (Timkiem_ChuoiCon(tenSach, tuKhoa)){
            if(soLuongKQ < MAX_DAUSACH) {
                ketQua[soLuongKQ++] = ds;
            }
        }
    }
}

// =================================================================
// 5. FILE I/O (ĐỌC - GHI FILE)
// =================================================================

// Hàm hỗ trợ ghi 1 đầu sách và các sách con của nó
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

// Lưu toàn bộ danh sách ra file
void ThuVienLogic::LuuFileDauSach() {
    ofstream fout("DauSach.txt");
    if (!fout) { emit guiThongBao("Lỗi: Không thể mở file DauSach.txt"); return; }
    for (int i = 0; i < this-> dsds.n; i++) {
        GhiDauSachVaoFile(fout, dsds.nodes[i]);
    }
    fout.close();
    qDebug() << "Đã lưu danh sách đầu sách.";
}

void ThuVienLogic::TaiFileDauSach() {
    ifstream fin("DauSach.txt");
    if(!fin) return;

    string line;
    DauSach* currentDS = nullptr;
    nodeSach* currentTail = nullptr; // <--- KHAI BÁO THÊM BIẾN NÀY ĐỂ GIỮ ĐUÔI

    while (getline(fin, line)) {
        if (line.empty()) {
            currentDS = nullptr;
            continue;
        }

        int soDauPhay = 0;
        for (char c : line) if (c == '|') soDauPhay++;
        stringstream ss(line);

        // TRƯỜNG HỢP 1: ĐỌC THÔNG TIN ĐẦU SÁCH
        if (soDauPhay == 6) {
            currentDS = new DauSach; string token;
            getline(ss, currentDS->ISBN, '|');
            getline(ss, currentDS->tensach, '|');
            getline(ss, token, '|'); currentDS->sotrang = stoi(token);
            getline(ss, currentDS->tacgia, '|');
            getline(ss, token, '|'); currentDS->namxb = stoi(token);
            getline(ss, currentDS->theloai, '|');
            getline(ss, token, '|'); currentDS->luotmuon = stoi(token);

            currentDS->tensach = ChuanHoaChuoi(currentDS->tensach);
            currentDS->tacgia = ChuanHoaChuoi(currentDS->tacgia);
            currentDS->theloai = ChuanHoaChuoi(currentDS->theloai);
            currentDS->firstDMS = NULL;

            // Đưa đầu sách vào mảng
            dsds.nodes[dsds.n++] = currentDS;

            // QUAN TRỌNG: Reset đuôi khi qua đầu sách mới
            currentTail = nullptr;
        }
        // TRƯỜNG HỢP 2: ĐỌC THÔNG TIN SÁCH CON (DMS)
        else if (soDauPhay == 2 && currentDS != nullptr) {
            nodeSach* newSach = new nodeSach; string token;
            getline(ss, newSach->sach.MASACH, '|');
            getline(ss, token, '|'); newSach->sach.trangthai = stoi(token);
            getline(ss, newSach->sach.vitri, '|');

            newSach->next = NULL; // Node mới luôn trỏ vào NULL (vì nó nằm cuối)

            // --- LOGIC SỬA ĐỔI: THÊM VÀO CUỐI (AddTail) ---
            if (currentDS->firstDMS == NULL) {
                // Nếu danh sách rỗng: Đầu là node mới, Đuôi cũng là node mới
                currentDS->firstDMS = newSach;
                currentTail = newSach;
            } else {
                // Nếu đã có danh sách: Nối đuôi cũ vào node mới
                currentTail->next = newSach;
                currentTail = newSach; // Cập nhật đuôi
            }
        }
    }
    fin.close();
    qDebug() << "Da tai " << dsds.n << " dau sach.";
}

// =================================================================
// 6. DỌN DẸP BỘ NHỚ (DESTRUCTOR HELPER)
// =================================================================

void ThuVienLogic::GiaiPhongDanhSachDauSach() {
    for (int i = 0; i < dsds.n; i++) {
        if (dsds.nodes[i] != nullptr) {
            // Giải phóng Danh sách liên kết sách con
            PTRDMS p = dsds.nodes[i]->firstDMS;
            while (p != nullptr) {
                PTRDMS temp = p;
                p = p->next;
                delete temp;
            }
            // Giải phóng struct đầu sách
            delete dsds.nodes[i];
            dsds.nodes[i] = nullptr;
        }
    }
    dsds.n = 0;
}
