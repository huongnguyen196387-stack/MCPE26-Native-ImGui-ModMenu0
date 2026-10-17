# Bedrock Client (Android)

Launcher Kotlin + `libclient.so` (C++17, Dobby, Dear ImGui GLES3).

## Build
- Cần mạng (CMake FetchContent kéo ImGui v1.91.5 và Dobby). CI: `.github/workflows/build_apk.yml`.
- Local: Android Studio (NDK 25.1.8937393, CMake 3.22.1) hoặc `gradle :app:assembleRelease` (chưa kèm gradlew; chạy `gradle wrapper` nếu muốn).

## Luồng dùng
1. Cài Minecraft bản sạch → mở app → **Ghi baseline**.
2. **Kiểm tra toàn vẹn & nạp client** (hash SHA-256 mọi `lib/<abi>/*.so` + chữ ký, rồi nạp libclient.so từ memfd).
3. Mã hóa payload (tùy chọn, chế độ chuẩn): `python tools/encrypt_client.py <libclient.so> <key_hex_64>`,
   build với `-PdevClientKeyHex=<key_hex_64>`. Không có `.enc` → DEV MODE (đọc libclient.so thô).

## Kiểm thử
- `tools/run_host_tests.sh` (chạy được trên Linux thường, CI chạy trước khi build APK): kiểm tra cú pháp toàn bộ native bằng header stub
  và test logic lõi (signature scanner + unique match, patch/restore Fullbright, LRU texture, SPSC ring, CPS) dưới ASan/UBSan.
- Nút **4. Trạng thái hook** trong app hiển thị hook nào đã cài (eglSwapBuffers, input, getFov, gamma, perspective, RenderLevel, GL).

## Chưa làm / chưa kiểm chứng
- Chưa biên dịch trên máy thật; API Dobby (target `dobby`, tùy chọn CMake) cần đối chiếu với commit bạn ghim.
- Hook chỉ có tác dụng khi game chạy CÙNG tiến trình với libclient.so → cần engine ảo hóa gắn vào `GameHost`.
- Signature/offset 1.26.50 là dữ liệu bạn cung cấp, chưa xác minh. Adapter Actor/Tessellator ở `game_internals.cpp` còn trống.
- Minecraft Bedrock Android nhận input qua Java/JNI nên `AInputQueue_getEvent` có thể không được gọi.

## GitHub Actions build

Pushes to `bedrock-client-import` run the APK build. The build job runs independently from host tests so an unrelated test failure does not prevent an APK artifact from being produced. The current host test suite has a failing wildcard-signature assertion (`tests/test_native.cpp`, line 44); this is reported separately and is not evidence that an APK build succeeded or failed.

After pushing, open **Actions → Build Android Client APK** and download the `BedrockClient-APK` artifact if the build job succeeds.
