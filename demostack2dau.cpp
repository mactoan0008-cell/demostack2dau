1. Module Quản Lý Sách (Đầu sách & Sách con)
Module này quản lý việc thêm, xóa, sửa, tìm kiếm và hiển thị danh sách sách.

A. Nhóm chức năng: Hiển thị & Lọc dữ liệu
Logic (ThuVienLogic):

LayDanhSachDauSach(...): Lấy danh sách đầu sách từ bộ nhớ, hỗ trợ sắp xếp theo thể loại.

SoSanhTheoTheLoaiVaTen(...): Hàm phụ trợ quy định logic sắp xếp (Thể loại trước -> Tên sách).

Giao diện (MainWindow):

capNhatBangDauSach(): Gọi logic lấy danh sách, lọc theo thể loại đang chọn (nếu có) và hiển thị lên bảng tableDauSach.

on_treeLoaiSach_itemClicked(...): Xử lý khi chọn một thể loại trên cây danh mục để lọc dữ liệu hiển thị.

B. Nhóm chức năng: Thêm mới & Cập nhật (Sửa)
Logic (ThuVienLogic):

ThemDauSach(...): Tạo đầu sách mới và chèn vào danh sách theo thứ tự tên.

HieuChinhDauSach(...): Cập nhật thông tin đầu sách và sắp xếp lại danh sách.

ThemSachCon(...): Tạo các cuốn sách con (bản sao vật lý) với mã cá biệt tự động tăng.

LaySoThuTuLonNhat(...): Tìm số đuôi lớn nhất của mã sách để sinh mã mới không trùng.

Giao diện (MainWindow):

on_btnTaoMoiSach_clicked(): Xóa trắng form để chuẩn bị nhập mới.

on_btnCapNhat_clicked(): Nút "Lưu" đa năng. Nếu là sách mới -> gọi ThemDauSach. Nếu là sách cũ -> gọi HieuChinhDauSach. Đồng thời tính toán để gọi ThemSachCon hoặc XoaBotSachTrongKho dựa trên số lượng nhập vào.

C. Nhóm chức năng: Xóa
Logic (ThuVienLogic):

XoaDauSach(...): Kiểm tra xem sách có ai mượn không. Nếu không, xóa toàn bộ sách con và đầu sách khỏi bộ nhớ.

XoaBotSachTrongKho(...): Chỉ xóa bớt số lượng sách con (khi cập nhật giảm số lượng), chỉ xóa cuốn trong kho/đã thanh lý.

Giao diện (MainWindow):

on_btnXoaDauSach_clicked(): Hiển thị hộp thoại xác nhận và gọi logic xóa. Thông báo lỗi nếu logic trả về kết quả đang có người mượn.

D. Nhóm chức năng: Tìm kiếm
Logic (ThuVienLogic):

TimDauSachTheoISBN(...): Tìm chính xác đầu sách theo mã ISBN.

TimSachTheoTen(...): Tìm kiếm gần đúng các sách có tên chứa từ khóa.

Giao diện (MainWindow):

on_btnTimKiem_clicked(): Lấy từ khóa từ ô nhập liệu, gọi logic tìm kiếm tương ứng và cập nhật lại bảng kết quả.

E. Nhóm chức năng: Chi tiết & Quản lý bản sao (Dialog)
Logic (ThuVienLogic):

TimSachConTheoMa(...): Tìm thông tin một cuốn sách cụ thể.

TimDocGiaDangMuonSach(...): Tìm độc giả đang giữ cuốn sách (để hiển thị tên người mượn).

ThanhLySach(...): Chuyển trạng thái sách sang "Thanh lý".

CapNhatViTriSach(...): Sửa vị trí kệ sách.

Giao diện (MainWindow):

on_tableDauSach_cellDoubleClicked(...): Mở cửa sổ Dialog chi tiết, hiển thị danh sách các cuốn sách con và người mượn. Tạo menu chuột phải để Thanh lý hoặc Sửa vị trí.

2. Module Thống Kê
Module này trích xuất dữ liệu báo cáo từ hệ thống.

A. Yêu cầu i: Thống kê độc giả quá hạn (Giảm dần theo thời gian)
Logic (ThuVienLogic):

LayDanhSachQuaHan(...): Hàm chính, trả về mảng các độc giả quá hạn đã được sắp xếp giảm dần theo số ngày quá hạn.

Duyet_DG_Muon_QuaHan(...): Hàm đệ quy duyệt cây nhị phân để tìm người vi phạm.

KTQuaHan(...) & Tinh_Khoang_Cach_Ngay(...): Các hàm tính toán ngày tháng để xác định vi phạm.

Giao diện (MainWindow):

on_btnThongKeQuaHan_clicked(): Lấy ngày mốc, gọi logic và hiển thị danh sách độc giả cùng số ngày quá hạn lên bảng tableQuaHan.

B. Yêu cầu j: Top 10 sách mượn nhiều nhất
Logic (ThuVienLogic):

LayTop10SachMuonNhieuNhat(...): Sắp xếp toàn bộ đầu sách theo luotmuon giảm dần và lấy 10 phần tử đầu tiên.

Giao diện (MainWindow):

on_btnTop10Sach_clicked(): Gọi logic lấy top 10 và hiển thị lên bảng tableTop10, định dạng cột lượt mượn cho nổi bật.
