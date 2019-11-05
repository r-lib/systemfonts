uc_data <- 'https://unicode.org/Public/emoji/12.1/emoji-data.txt'
cp <- read.table(uc_data, header = FALSE, sep = ';', stringsAsFactors = FALSE)
cp <- split(strsplit(trimws(cp[[1]]), '..', fixed = TRUE), trimws(cp[[2]]))
emoji <- unlist(lapply(cp$Emoji, function(x) {
  x <- strtoi(paste0('0x', x))
  if (length(x) == 1) return(x)
  seq.int(x[1], x[2])
}))
pres <- unlist(lapply(cp$Emoji_Presentation, function(x) {
  x <- strtoi(paste0('0x', x))
  if (length(x) == 1) return(x)
  seq.int(x[1], x[2])
}))
text_pres_emoji <- setdiff(emoji, pres)
base_mod <- unlist(lapply(cp$Emoji_Modifier_Base, function(x) {
  x <- strtoi(paste0('0x', x))
  if (length(x) == 1) return(x)
  seq.int(x[1], x[2])
}))
base_mod_emoji <- intersect(text_pres, base_mod)
all_emoji <- unique(unlist(lapply(unlist(cp, recursive = FALSE, use.names = FALSE), function(x) {
  x <- strtoi(paste0('0x', x))
  if (length(x) == 1) return(x)
  seq.int(x[1], x[2])
})))
all_emoji <- as.integer(all_emoji)
text_pres_emoji <- as.integer(text_pres_emoji)
base_mod_emoji <- as.integer(base_mod_emoji)

save(all_emoji, text_pres_emoji, base_mod_emoji, file = 'R/sysdata.rda')

rm(uc_data, cp, emoji, pres, text_pres_emoji, base_mod, base_mod_emoji, all_emoji)


