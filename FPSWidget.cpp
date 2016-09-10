//
// Created by Karol on 03.09.2016.
//

#include "error_log.hpp"
#include "FPSWidget.hpp"

FPSWidget::FPSWidget() = default;
FPSWidget::~FPSWidget() = default;

bool FPSWidget::init(SDL_Renderer * render) noexcept {
    SDL_Texture_uptr tex{SDL_CreateTexture(render, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, m_width, m_height)};
    if (!tex) {
        printfe("Couldn't create texture: %s", SDL_GetError());
        return false;
    }

    if (SDL_SetTextureBlendMode(tex.get(), SDL_BLENDMODE_BLEND) != 0) {
        printfe("Couldn't set texture blend mode: %s", SDL_GetError());
        return false;
    }

    m_buffer = ::std::move(tex);

    return true;
}
void FPSWidget::deinit() noexcept {
    m_buffer = nullptr;
}

static
double closestMinPrettyFPS(double fps) noexcept {
#define ElIfFPSInRange(beg,end) else if (beg <= fps && fps < end) return beg
    if (fps > 60.0)
        return (unsigned)floor(fps / 60.0) * 60u;
    ElIfFPSInRange(30.0, 60.0);
    ElIfFPSInRange(20.0, 30.0);
    ElIfFPSInRange(15.0, 20.0);
    ElIfFPSInRange(12.0, 15.0);
    ElIfFPSInRange(10.0, 12.0);
    ElIfFPSInRange( 6.0, 10.0);
    ElIfFPSInRange( 5.0,  6.0);
    ElIfFPSInRange( 3.0,  5.0);
    ElIfFPSInRange( 2.0,  3.0);
    ElIfFPSInRange( 1.0,  2.0);
#undef ElIfFPSInRange

    return 1.0;
}

void FPSWidget::update(double dt) noexcept {
    m_dt_mgr.update(dt);

    const double destMaxDT = 1000.0 / closestMinPrettyFPS(1000.0 / m_dt_mgr.max());
    //const double destMaxDT = m_dt_mgr.max();

    const double fpsDiff = 1000.0 / destMaxDT - 1000.0 / m_destMaxDT;

    if (fabs(fpsDiff) > 0.5) {
        m_destMaxDT = destMaxDT;
        const double distMaxDT = destMaxDT - m_currMaxDT;
        m_maxDTVel = distMaxDT / 1500.0;
    }

    if (m_maxDTVel != 0.0) {
        const double maxDTStep = m_maxDTVel * dt;
        const double newMaxDT = m_currMaxDT + maxDTStep;

        if (   (maxDTStep > 0.0 && newMaxDT >= destMaxDT)
               || (maxDTStep < 0.0 && newMaxDT <= destMaxDT)) {
            m_currMaxDT = destMaxDT;
            m_maxDTVel = 0.0;
        }
        else
            m_currMaxDT = newMaxDT;
    }
}

void FPSWidget::updateTexture(SDL_Renderer * renderer, FontMenager& fnt_mgr, unsigned fid) noexcept {
    // TODO: Error handling
    SDL_Texture * m_lpOldTarget = SDL_GetRenderTarget(renderer);
    SDL_Color old_color;
    SDL_GetRenderDrawColor(renderer, &old_color.r, &old_color.g, &old_color.b, &old_color.a);

    SDL_SetRenderTarget(renderer, m_buffer.get());
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_RenderClear(renderer);

    auto u32snprintfA = [](char32_t * __stream, size_t __n, const char *__format, ...) {
        char buffer[__n+1];
        va_list __local_argv;
        va_start(__local_argv, __format);
        const int result = vsnprintf(buffer, __n, __format, __local_argv);
        va_end(__local_argv);
        if (result > 0) {
            const size_t out_size = std::min(__n, (size_t)result);
            for (size_t i = 0; i < out_size; ++i)
                __stream[i] = char32_t(buffer[i]);
        };

        return result;
    };

    // Begin of string preparation
    char32_t msMaxStrBegin[8];
    char32_t * const msMaxStrEnd = msMaxStrBegin + u32snprintfA(msMaxStrBegin, 8, "%0.2f", m_currMaxDT);

    char32_t msMedStrBegin[8];
    char32_t * const msMedStrEnd = msMedStrBegin + u32snprintfA(msMedStrBegin, 8, "%0.2f", m_currMaxDT/2.0);

    static const char32_t msMinStrBegin[] = U"0.00";
    static constexpr const char32_t * const msMinStrEnd = msMinStrBegin + 4;

    static const char32_t fpsMaxStrBegin[] = U"\u221E";
    static constexpr const char32_t * const fpsMaxStrEnd = fpsMaxStrBegin + 1;

    static const char32_t fpsStrBegin[] = U"FPS";
    static constexpr const char32_t * const fpsStrEnd = fpsStrBegin + 3;

    static const char32_t msStrBegin[] = U"[ms]";
    static constexpr const char32_t * const msStrEnd = msStrBegin + 4;

    char32_t fpsMedStrBegin[8];
    char32_t * const fpsMedStrEnd = fpsMedStrBegin + u32snprintfA(fpsMedStrBegin, 8, "%0.1f", 1000.0 / (m_currMaxDT / 2.0));

    char32_t fpsMinStrBegin[8];
    char32_t * const fpsMinStrEnd = fpsMinStrBegin + u32snprintfA(fpsMinStrBegin, 8, "%0.1f", 1000.0 / m_currMaxDT);
    // End of string preparation

    constexpr unsigned margin = 5u;
    Font fnt{fid, 12u, {255u, 255u, 255u}};
    FontCache& cache = fnt_mgr.getCache(fnt.fid);
    const int fontHeight = cache.lineHeight(fnt.size) >> 6;
    SDL_Texture * texture = cache.texture();
    SDL_SetTextureColorMod(texture, fnt.color.r, fnt.color.g, fnt.color.b);

    const int msMaxStrWidth = fnt_mgr.getRAWTextWidth(fnt.fid, fnt.size, msMaxStrBegin, msMaxStrEnd);
    const int msMidStrWidth = fnt_mgr.getRAWTextWidth(fnt.fid, fnt.size, msMedStrBegin, msMedStrEnd);
    const int msMinStrWidth = fnt_mgr.getRAWTextWidth(fnt.fid, fnt.size, msMinStrBegin, msMinStrEnd);
    const int msStrsWidth = ::std::max({msMaxStrWidth, msMidStrWidth, msMinStrWidth});

    const int fpsMaxStrWidth = fnt_mgr.getRAWTextWidth(fnt.fid, fnt.size, fpsMaxStrBegin, fpsMaxStrEnd);
    const int fpsMidStrWidth = fnt_mgr.getRAWTextWidth(fnt.fid, fnt.size, fpsMedStrBegin, fpsMedStrEnd);
    const int fpsMinStrWidth = fnt_mgr.getRAWTextWidth(fnt.fid, fnt.size, fpsMinStrBegin, fpsMinStrEnd);
    const int fpsStrsWidth = ::std::max({fpsMaxStrWidth, fpsMidStrWidth, fpsMinStrWidth});

    const int msStrWidth = fnt_mgr.getRAWTextWidth(fnt.fid, fnt.size, msStrBegin, msStrEnd);

    const int axisYTop = margin + fontHeight/2;
    const int axisYBottom = m_height - margin - fontHeight/2 - margin - fontHeight - margin;
    const int axisYHalf = (axisYTop + axisYBottom) / 2;
    const int axisYHeight = axisYBottom - axisYTop;

    const int axisYMin = axisYBottom - (int)(axisYHeight * (m_dt_mgr.min() / m_currMaxDT) + 0.5);
    const int axisYMax = axisYBottom - (int)(axisYHeight * (m_dt_mgr.max() / m_currMaxDT) + 0.5);
    const int axisYAvg = axisYBottom - (int)(axisYHeight * (m_dt_mgr.averange() / m_currMaxDT) + 0.5);

    auto drawVAxis = [axisYTop, axisYHalf, axisYBottom](SDL_Renderer * renderer, int left, int right){
        const int top = axisYTop, bottom = axisYBottom, vMiddle = axisYHalf;
        const int hMiddle = (left + right) / 2;

        SDL_RenderDrawLine(renderer, hMiddle, top, hMiddle, bottom);
        SDL_RenderDrawLine(renderer, left, top, right, top);
        SDL_RenderDrawLine(renderer, left, vMiddle, right, vMiddle);
        SDL_RenderDrawLine(renderer, left, bottom, right, bottom);
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    const int leftAxisLeft = (int)margin + fpsStrsWidth + (int)margin;
    const int leftAxisRight = leftAxisLeft + 3;
    drawVAxis(renderer, leftAxisLeft, leftAxisRight);

    const int rightAxisLeft = m_width - (int)margin - msStrsWidth - (int)margin - 3;
    const int rightAxisRight = rightAxisLeft + 3;
    drawVAxis(renderer, rightAxisLeft, rightAxisRight);

    fnt_mgr.drawRAWText(renderer, texture, cache, fnt.size, fpsMinStrBegin, fpsMinStrEnd, leftAxisLeft - (int)margin - fpsMinStrWidth, axisYTop + fontHeight / 2u);
    fnt_mgr.drawRAWText(renderer, texture, cache, fnt.size, fpsMedStrBegin, fpsMedStrEnd, leftAxisLeft - (int)margin - fpsMidStrWidth, axisYHalf + fontHeight / 2u);
    fnt_mgr.drawRAWText(renderer, texture, cache, fnt.size, fpsMaxStrBegin, fpsMaxStrEnd, leftAxisLeft - (int)margin - fpsMaxStrWidth, axisYBottom + fontHeight / 2u);

    fnt_mgr.drawRAWText(renderer, texture, cache, fnt.size, msMaxStrBegin, msMaxStrEnd, rightAxisRight + (int)margin, axisYTop + fontHeight / 2u);
    fnt_mgr.drawRAWText(renderer, texture, cache, fnt.size, msMedStrBegin, msMedStrEnd, rightAxisRight + (int)margin, axisYHalf + fontHeight / 2u);
    fnt_mgr.drawRAWText(renderer, texture, cache, fnt.size, msMinStrBegin, msMinStrEnd, rightAxisRight + (int)margin, axisYBottom + fontHeight / 2u);

    fnt_mgr.drawRAWText(renderer, texture, cache, fnt.size, fpsStrBegin, fpsStrEnd, leftAxisRight + (int)margin, axisYTop + fontHeight / 2u);
    fnt_mgr.drawRAWText(renderer, texture, cache, fnt.size, msStrBegin, msStrEnd, rightAxisLeft - (int)margin - msStrWidth, axisYTop + fontHeight / 2u);

    const int chartLeft = leftAxisRight + (int)margin;
    const int chartRight = rightAxisLeft - (int)margin;
    const SDL_Rect chartRC = { chartLeft, axisYTop, chartRight - chartLeft, axisYHeight };

    const double samples_per_line = (double)m_dt_mgr.capacity / (double)chartRC.w;

    for (unsigned i = 0; i < (unsigned)chartRC.w; ++i) {
        double dt_sum = 0u;
        double dt_min = ::std::numeric_limits<double>::max();
        double dt_max = ::std::numeric_limits<double>::min();
        for (unsigned j = 0; j < (unsigned)ceil(samples_per_line); ++j) {
            const double sample = m_dt_mgr.samples()[i * (unsigned)floor(samples_per_line) + j];
            dt_sum += sample;
            dt_min = ::std::min(dt_min, sample);
            dt_max = ::std::max(dt_max, sample);
        }
        const double dt_avg = dt_sum / floor(samples_per_line);

        const double dt_percent_min = dt_min / m_currMaxDT;
        const double dt_percent_max = dt_max / m_currMaxDT;
        const double dt_percent_avg = dt_avg / (dt_avg - m_dt_mgr.max()) / (m_dt_mgr.max() - m_dt_mgr.min());
        const Uint8 r = (Uint8)::std::min(static_cast<unsigned>(dt_percent_avg * 255.0), 255u);
        const Uint8 g = (Uint8)::std::min(static_cast<unsigned>((1.0 - dt_percent_avg) * 255.0), 255u);

        const unsigned y_min = static_cast<unsigned>((1.0 - dt_percent_min) * chartRC.h);
        const unsigned y_max = static_cast<unsigned>((1.0 - dt_percent_max) * chartRC.h);

        if (y_min == y_max) {
            if (y_min != chartRC.h) {
                SDL_SetRenderDrawColor(renderer, r, g, 0, 200);
                SDL_RenderDrawPoint(renderer, i + chartRC.x, y_max + chartRC.y);
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 200);
                SDL_RenderDrawLine(renderer, i + chartRC.x, y_max-1 + chartRC.y, i + chartRC.x, chartRC.h + chartRC.y);
            }
        }
        else {
            SDL_SetRenderDrawColor(renderer, r, g, 0, 128);
            SDL_RenderDrawLine(renderer, i + chartRC.x, y_max + chartRC.y, i + chartRC.x, y_min + chartRC.y);

            if (y_min != chartRC.h) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 200);
                SDL_RenderDrawLine(renderer, i + chartRC.x, y_min + chartRC.y, i + chartRC.x, chartRC.h + chartRC.y);
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, leftAxisLeft, axisYMin, rightAxisRight, axisYMin);
    SDL_SetRenderDrawColor(renderer, 255, 128, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, leftAxisLeft, axisYAvg, rightAxisRight, axisYAvg);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, leftAxisLeft, axisYMax, rightAxisRight, axisYMax);

    const int textTop = axisYBottom + fontHeight/2 + margin;

    char32_t buffer[64];
    auto fixBuffer = [&buffer](){ for (char32_t& c : buffer) if (c == U'-') c = U'\u2026'; };
    int buflen = u32snprintfA(buffer, 64, "%0.1f-%0.1f-%0.1f", 1000.0 / m_dt_mgr.max(),
                              1000.0 / m_dt_mgr.averange(),
                              1000.0 / m_dt_mgr.min());
    fixBuffer();
    const std::u32string fpsDesc(buffer, buflen);
    buflen = u32snprintfA(buffer, 64, "%0.2f-%0.2f-%0.2f", m_dt_mgr.min(), m_dt_mgr.averange(), m_dt_mgr.max());
    fixBuffer();
    const std::u32string msDesc(buffer, buflen);
    buflen = u32snprintfA(buffer, 64, "%u", m_dt_mgr.frames());
    const std::u32string framesDesc(buffer, buflen);


    fnt.color = { 255, 255, 0 };
    fnt_mgr.drawText(renderer, fnt, fpsDesc, margin, textTop, HA_LEFT | VA_TOP);
    fnt_mgr.drawText(renderer, fnt, framesDesc, m_width / 2u, textTop, HA_CENTER | VA_TOP);
    fnt_mgr.drawText(renderer, fnt, msDesc, m_width - margin, textTop, HA_RIGHT | VA_TOP);

    SDL_SetRenderDrawColor(renderer, old_color.r, old_color.g, old_color.b, old_color.a);
    SDL_SetRenderTarget(renderer, m_lpOldTarget);
}