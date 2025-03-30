class WaveManager {
public:

    float m_speedMultiplier;
    float m_healthMultiplier;
    float m_difficultyTimer;
    
    // Constants cho scaling
    static constexpr float DIFFICULTY_INCREASE_INTERVAL = 10.0f; // Tăng độ khó mỗi 10 giây
    static constexpr float SPEED_INCREASE_RATE = 0.1f; // Tăng 10% speed mỗi lần
    static constexpr float HEALTH_INCREASE_RATE = 0.15f; // Tăng 15% health mỗi lần
};