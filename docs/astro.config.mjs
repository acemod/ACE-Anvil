import { defineConfig } from 'astro/config'
import starlight from '@astrojs/starlight'
import tailwind from '@astrojs/tailwind'

import { config } from './src/utils/config'

// https://astro.build/config
export default defineConfig({
  integrations: [
    starlight({
      title: config.site.name,
      customCss: [
        // Path to your Tailwind base styles:
        './src/styles/tailwind.css',
      ],
      social: {
        'discord': 'https://acemod.org/discord',
        'github': 'https://github.com/acemod/ACE-Anvil',
        'x.com': 'https://twitter.com/ACE3Mod',
        'facebook': 'https://facebook.com/ACE3Mod',
        'youtube': 'https://youtube.com/c/ACE3Mod',
      },
      sidebar: [
        {
          label: 'Guides',
          autogenerate: { directory: 'guides' },
          // items: [
          //  // Each item here is one entry in the navigation menu.
          //  { label: 'Example Guide', link: '/guides/example/' },
          //  { label: 'Development', link: '/guides/development/' },
          // ],
        },
        {
          label: 'Features',
          autogenerate: { directory: 'features' },
        },
        {
          label: 'Reference',
          autogenerate: { directory: 'reference' },
        },
      ],
    }),
    tailwind({
      // Disable the default base styles:
      applyBaseStyles: false,
    }),
  ],
})
