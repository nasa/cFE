-- -- fix-code-blocks.lua
--
-- function CodeBlock(elem)
--   return pandoc.Emph{pandoc.Str 'Hello Again'}
-- end
-- --
function CodeBlock(block)
  block.attr = {id = '', class = 'c'}
  -- Surround all images with image-centering raw LaTeX.
    return {
      pandoc.RawBlock('latex', '\\begingroup\\centering\\fontsize{8pt}{8pt}\\selectfont'),
      block,
      pandoc.RawBlock('latex', '\\endgroup')
    }
end

-- TODO: center images, TODO:shrik table font size, TODO: make code-block language override conditional, TODO: move shell script attributes to filter
