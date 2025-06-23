import { defineConfig } from 'astro/config'
import starlight from '@astrojs/starlight'
import tailwindcss from '@tailwindcss/vite'
import remarkMath from 'remark-math'
import rehypeMathML from '@daiji256/rehype-mathml'

import { config } from './src/utils/config'

// https://astro.build/config
export default defineConfig({
  site: config.site.url,
  base: process.env.BASE_URL || '',
  markdown: {
    remarkPlugins: [remarkMath],
    rehypePlugins: [rehypeMathML],
  },

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
      social: [
        { icon: 'discord', label: 'Discord', href: config.ace.discordUrl },
        { icon: 'github', label: 'GitHub', href: config.ace.githubUrl },
        { icon: 'x.com', label: 'X', href: 'https://x.com/ACE3Mod' },
        { icon: 'facebook', label: 'Facebook', href: 'https://facebook.com/ACE3Mod' },
        { icon: 'youtube', label: 'YouTube', href: 'https://youtube.com/c/ACE3Mod' },
      ],
      sidebar: [
        {
          label: 'Overview',
          items: [
            { label: 'General Information', link: '/general/' },
            { label: 'Frequently Asked Questions', link: '/faq/' },
            { label: 'Team', link: '/team/' },
            { label: 'Branding', link: '/branding/' },
          ],
        },
        // {
        //  label: 'News',
        //  collapsed: true,
        //  autogenerate: { directory: 'news' },
        // },
        {
          label: 'Guides',
          autogenerate: { directory: 'guides' },
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
        baseUrl: `${config.ace.githubUrl}/edit/master/docs/`,
      },
      lastUpdated: true,
    }),
  ],

  vite: {
    plugins: [tailwindcss({
      // Disable the default base styles:
      applyBaseStyles: false,
    })],
    server: {
      watch: {
        usePolling: false,
      },
    },
  },
})
