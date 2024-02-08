#include "GaussianBlurShader.hpp"
#include "rendering/Texture.hpp"

#include <fmt/format.h>

#include <array>

namespace llengine {
constexpr std::string_view VERTICAL_BLUR_VERT_SHADER_TEXT =
    #include "shaders/vertical_gaussian_blur.vert"
;
constexpr std::string_view VERTICAL_BLUR_FRAG_SHADER_TEXT =
    #include "shaders/vertical_gaussian_blur.frag"
;
constexpr std::string_view HORIZONTAL_BLUR_VERT_SHADER_TEXT =
    #include "shaders/horizontal_gaussian_blur.vert"
;
constexpr std::string_view HORIZONTAL_BLUR_FRAG_SHADER_TEXT =
    #include "shaders/horizontal_gaussian_blur.frag"
;

std::array<float, 128 * 3> GAUSS_BLUR_KERNEL = {
    1.0f, 0.0f, 0.0f, 0.99966305354936f, 0.0f, 0.0f, 0.9986528952418986f, 0.0f, 0.0f, 0.9969715659172665f, 0.0f, 0.0f, 0.9946224593399464f, 0.0f, 0.0f, 0.9916103107846291f, 0.0f, 0.0f, 0.9879411811310628f, 0.0f, 0.0f, 0.9836224365431518f, 0.0f, 0.0f, 0.9786627238276675f, 0.0f, 0.0f, 0.9730719415879434f, 0.0f, 0.0f, 0.966861207307249f, 0.0f, 0.0f, 0.9600428205150398f, 0.0f, 0.0f, 0.9526302222068787f, 0.0f, 0.0f, 0.9446379507053932f, 0.0f, 0.0f, 0.9360815941651021f, 0.0f, 0.0f, 0.9269777399382013f, 0.0f, 0.0f, 0.9173439210314008f, 0.0f, 0.0f, 0.9071985598955354f, 0.0f, 0.0f, 0.8965609097999341f, 0.0f, 0.0f, 0.8854509940523131f, 0.0f, 0.0f, 0.8738895433322726f, 0.0f, 0.0f, 0.8618979314122621f, 0.0f, 0.0f, 0.8494981095441337f, 0.0f, 0.0f, 0.8367125397921192f, 0.0f, 0.0f, 0.8235641275942354f, 0.0f, 0.0f, 0.810076153833777f, 0.0f, 0.0f, 0.7962722067007036f, 0.0f, 0.0f, 0.7821761136194045f, 0.0f, 0.0f, 0.7678118735145847f, 0.0f, 0.0f, 0.7532035896808922f, 0.0f, 0.0f, 0.7383754035144766f, 0.0f, 0.0f, 0.7233514293559833f, 0.0f, 0.0f, 0.7081556906846399f, 0.0f, 0.0f, 0.692812057892144f, 0.0f, 0.0f, 0.6773441878531128f, 0.0f, 0.0f, 0.6617754654959939f, 0.0f, 0.0f, 0.6461289475646599f, 0.0f, 0.0f, 0.6304273087465092f, 0.0f, 0.0f, 0.6146927903278898f, 0.0f, 0.0f, 0.5989471515221371f, 0.0f, 0.0f, 0.5832116235995887f, 0.0f, 0.0f, 0.5675068669327086f, 0.0f, 0.0f, 0.5518529310530267f, 0.0f, 0.0f, 0.5362692178000774f, 0.0f, 0.0f, 0.5207744476260079f, 0.0f, 0.0f, 0.5053866291031217f, 0.0f, 0.0f, 0.4901230316654133f, 0.0f, 0.0f, 0.47500016159923913f, 0.0f, 0.0f, 0.46003374128273566f, 0.0f, 0.0f, 0.4452386916585209f, 0.0f, 0.0f, 0.43062911790967934f, 0.0f, 0.0f, 0.41621829829510393f, 0.0f, 0.0f, 0.4020186760870068f, 0.0f, 0.0f, 0.38804185454088835f, 0.0f, 0.0f, 0.37429859481650024f, 0.0f, 0.0f, 0.3607988167574191f, 0.0f, 0.0f, 0.3475516024267854f, 0.0f, 0.0f, 0.334565202287588f, 0.0f, 0.0f, 0.32184704390762764f, 0.0f, 0.0f, 0.30940374306195984f, 0.0f, 0.0f, 0.29724111709924705f, 0.0f, 0.0f, 0.28536420043300237f, 0.0f, 0.0f, 0.2737772620142168f, 0.0f, 0.0f, 0.26248382463829634f, 0.0f, 0.0f, 0.2514866859365872f, 0.0f, 0.0f, 0.24078794090101344f, 0.0f, 0.0f, 0.23038900578947594f, 0.0f, 0.0f, 0.22029064325961295f, 0.0f, 0.0f, 0.2104929885792913f, 0.0f, 0.0f, 0.20099557676372837f, 0.0f, 0.0f, 0.1917973704913977f, 0.0f, 0.0f, 0.18289678865380868f, 0.0f, 0.0f, 0.1742917353978204f, 0.0f, 0.0f, 0.16597962952329973f, 0.0f, 0.0f, 0.15795743410361632f, 0.0f, 0.0f, 0.15022168620163473f, 0.0f, 0.0f, 0.14276852655944994f, 0.0f, 0.0f, 0.13559372914607865f, 0.0f, 0.0f, 0.12869273045360402f, 0.0f, 0.0f, 0.12206065843882186f, 0.0f, 0.0f, 0.11569236101420458f, 0.0f, 0.0f, 0.10958243399893344f, 0.0f, 0.0f, 0.10372524844779765f, 0.0f, 0.0f, 0.09811497728287413f, 0.0f, 0.0f, 0.09274562116004462f, 0.0f, 0.0f, 0.08761103350952332f, 0.0f, 0.0f, 0.08270494469662591f, 0.0f, 0.0f, 0.0780209852559701f, 0.0f, 0.0f, 0.0735527081591184f, 0.0f, 0.0f, 0.06929361008232857f, 0.0f, 0.0f, 0.06523715164753342f, 0.0f, 0.0f, 0.061376776615903655f, 0.0f, 0.0f, 0.057705930019327875f, 0.0f, 0.0f, 0.054218075220860314f, 0.0f, 0.0f, 0.05090670990060917f, 0.0f, 0.0f, 0.04776538096866327f, 0.0f, 0.0f, 0.04478769841146291f, 0.0f, 0.0f, 0.041967348082505455f, 0.0f, 0.0f, 0.03929810345242863f, 0.0f, 0.0f, 0.03677383633733631f, 0.0f, 0.0f, 0.03438852662771154f, 0.0f, 0.0f, 0.03213627104341055f, 0.0f, 0.0f, 0.030011290943044017f, 0.0f, 0.0f, 0.02800793921853899f, 0.0f, 0.0f, 0.026120706307838026f, 0.0f, 0.0f, 0.02434422536054304f, 0.0f, 0.0f, 0.022673276592859448f, 0.0f, 0.0f, 0.02110279086944988f, 0.0f, 0.0f, 0.019627852550781928f, 0.0f, 0.0f, 0.018243701645262168f, 0.0f, 0.0f, 0.016945735305902775f, 0.0f, 0.0f, 0.015729508711484572f, 0.0f, 0.0f, 0.014590735372173341f, 0.0f, 0.0f, 0.013525286899333602f, 0.0f, 0.0f, 0.012529192278879508f, 0.0f, 0.0f, 0.011598636686922166f, 0.0f, 0.0f, 0.010729959885734537f, 0.0f, 0.0f, 0.009919654237172776f, 0.0f, 0.0f, 0.009164362369682313f, 0.0f, 0.0f, 0.008460874533895513f, 0.0f, 0.0f, 0.007806125680608074f, 0.0f, 0.0f, 0.007197192293618057f, 0.0f, 0.0f, 0.006631289008540478f, 0.0f, 0.0f, 0.006105765047282657f, 0.0f, 0.0f, 0.005618100496394254f, 0.0f, 0.0f, 0.005165902456004643f, 0.0f, 0.0f, 0.004746901084537545f, 0.0f, 0.0f, 0.00435894556286112f, 0.0f, 0.0f
};

GaussianBlurShader::GaussianBlurShader() :
    hor_blur_shader(HORIZONTAL_BLUR_VERT_SHADER_TEXT, HORIZONTAL_BLUR_FRAG_SHADER_TEXT),
    vert_blur_shader(VERTICAL_BLUR_VERT_SHADER_TEXT, VERTICAL_BLUR_FRAG_SHADER_TEXT),
    gauss_weights(Texture::from_pixel_data<float>(
        GAUSS_BLUR_KERNEL.data(), glm::u32vec2(128, 1), Texture::Type::TEX_1D, Texture::Format::R11G11B10F
    )) {}

void GaussianBlurShader::use_horizontal_shader(const Texture& source, float radius, float step_coefficient, float source_mipmap_level) const {
    if (source.get_type() != Texture::Type::TEX_2D) {
        throw std::runtime_error("It is impossible to use a non-2D texture as a source for a blur shader");
    }

    float hor_step = 1.0f / source.get_size().x * step_coefficient;
    std::int32_t samples_to_take = radius / hor_step;
    float coefficient = 1.0f / samples_to_take;

    hor_blur_shader.use_shader();
    hor_blur_shader.set_float<"radius">(radius);
    hor_blur_shader.set_float<"hor_step">(hor_step);
    hor_blur_shader.bind_1d_texture<"gauss_weights">(gauss_weights.get_id(), 0);
    hor_blur_shader.bind_2d_texture<"source_texture">(source.get_id(), 1);
    hor_blur_shader.set_float<"coefficient">(coefficient);
    hor_blur_shader.set_int<"samples_to_take">(samples_to_take);
    hor_blur_shader.set_float<"source_mipmap">(source_mipmap_level);
}

void GaussianBlurShader::use_vertical_shader(const Texture& source, float radius, float step_coefficient) const {
    if (source.get_type() != Texture::Type::TEX_2D) {
        throw std::runtime_error("It is impossible to use a non-2D texture as a source for a blur shader");
    }

    float vert_step = 1.0f / source.get_size().y * step_coefficient;
    float coefficient = 1.0f / std::floor(radius / vert_step);

    vert_blur_shader.use_shader();
    vert_blur_shader.set_float<"radius">(radius);
    vert_blur_shader.set_float<"vert_step">(vert_step);
    vert_blur_shader.bind_1d_texture<"gauss_weights">(gauss_weights.get_id(), 0);
    vert_blur_shader.bind_2d_texture<"source_texture">(source.get_id(), 1);
    vert_blur_shader.set_float<"coefficient">(coefficient);
}
}