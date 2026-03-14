idf_component_register(SRCS
        "nvs/nvs_store.cpp"
        PRIV_REQUIRES result nvs_flash
        INCLUDE_DIRS "."
)
