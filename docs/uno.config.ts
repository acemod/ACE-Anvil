import presetWebFonts from '@unocss/preset-web-fonts'

import { defineConfig, presetAttributify, presetIcons, presetTypography, presetUno, transformerDirectives, transformerVariantGroup } from 'unocss'

export default defineConfig({
  presets: [
    presetUno(),
    presetAttributify(),
    presetIcons({ scale: 1.2 }),
    presetTypography(),
    presetWebFonts(
      //{

    //   provider: 'bunny',
    //   fonts: {
    //     sans: 'Inter',
    //     mono: ['Fira Code', 'Fira Mono:400,700'],
    //   },

    // }
    ),
  ],
  transformers: [transformerDirectives(), transformerVariantGroup()],
  // theme: {
  //   colors: {
  //     primary: 'oklch(0.59 0.21 5.28)',
  //   },
  // },

})
