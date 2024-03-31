import { defineConfig } from 'astro/config'
import starlight from '@astrojs/starlight'
import tailwind from '@astrojs/tailwind'

import { config } from './src/utils/config'

// https://astro.build/config
export default defineConfig({
  site: 'https://anvil.acemod.org',
  integrations: [
    starlight({
      title: config.site.name,
      logo: {
        dark: './src/assets/logo-white.webp',
        light: './src/assets/logo-black.webp',
        replacesTitle: true,
      },
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
        //{
        //  label: 'News',
        //  collapsed: true,
        //  autogenerate: { directory: 'news' },
        //},
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
          label: 'Components',
          autogenerate: { directory: 'components' },
        },
      ],
      locales: {
        root: {
          label: 'English',
          lang: 'en',
        },
      },
      editLink: {
        baseUrl: 'https://github.com/acemod/ACE-Anvil/edit/master/docs/',
      },
      lastUpdated: true,
    }),
    tailwind({
      // Disable the default base styles:
      applyBaseStyles: false,
    }),
  ],
})
